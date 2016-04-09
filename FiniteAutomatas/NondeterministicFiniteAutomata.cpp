#include "PCH.h"
#include "NondeterministicFiniteAutomata.h"

NondeterministicFiniteAutomata::NondeterministicFiniteAutomata(std::ifstream& ifs)
{
	uint32_t finalStates;
	ifs >> _states >> _initialState >> finalStates;
	assert(_states >= 0 && (_initialState >= 0 && _initialState < _states) && (finalStates >= 0 && finalStates <= _states));

	for (uint32_t i = 0; i < finalStates; i++)
	{
		uint32_t finalState;
		ifs >> finalState;
		_finalStates.push_back(finalState);
	}

	while (!ifs.eof())
	{
		uint32_t currentState, nextState;
		char key;

		ifs >> currentState >> key >> nextState;

		TransitionPair pair(currentState, key);
		TransitionMap::iterator itr = _transitionFunction.find(pair);

		if (itr == _transitionFunction.end())
		{
			Vector<uint32_t> transitionState;
			transitionState.push_back(nextState);
			_transitionFunction.emplace(pair, transitionState);
		}
		else
			itr->second.push_back(nextState);
	}
}

bool NondeterministicFiniteAutomata::isAccepted(const String& word) const
{
	if (!_states || _finalStates.empty() || _transitionFunction.empty())
		return false;

	if (word.empty() && isFinalState(lambdaClosure(_initialState)))
		return true;

	if (ToDFA().isAccepted(word))
		return true;

	return false;
}

String NondeterministicFiniteAutomata::generateWord(const uint32_t& length) const
{
	if (!_states || _transitionFunction.empty() || _finalStates.empty() || !length)
		return String();

	String word = ToDFA().generateWord(length);

	return word;
}

DFA NondeterministicFiniteAutomata::ToDFA() const
{
	// We don't check for finalStates
	if (!_states || _transitionFunction.empty())
		return DFA();

	const Set<char> alphabet = getAlphabet();
	Vector<Set<uint32_t>> States, FinalStates;
	Set<uint32_t> InitialState(lambdaClosure(_initialState));
	Map<Pair<Set<uint32_t>, char>, Set<uint32_t>> TransitionFunction;
	States.push_back(InitialState);

	// Cannot use iterator because we constantly add elements in States.
	for (uint32_t i = 0; i < States.size(); i++)
	{
		for (Set<char>::const_iterator key = alphabet.begin(); key != alphabet.end(); key++)
		{
			Set<uint32_t> _state = lambdaClosure(moveTo(States[i], *key));

			if ((std::find(States.begin(), States.end(), _state) == States.end()) && !_state.empty())
				States.push_back(_state);

			if (!_state.empty())
				TransitionFunction[Pair<Set<uint32_t>, char>(States[i], *key)] = _state;
		}
	}

	for (Vector<Set<uint32_t>>::const_iterator itr = States.begin(); itr != States.end(); itr++)
	{
		for (Set<uint32_t>::const_iterator _itr = itr->begin(); _itr != itr->end(); _itr++)
		{
			if (isFinalState((*_itr)))
			{
				FinalStates.push_back(*itr);
				break;
			}
		}
	}

	uint32_t states = States.size(), initialState = 0;
	Vector<uint32_t> finalStates;
	TransitionMap transitionFunction;

	for (Vector<Set<uint32_t>>::const_iterator itr = FinalStates.begin(); itr != FinalStates.end(); itr++)
	{
		for (uint32_t i = 0; i < States.size(); i++)
			if (States[i] == (*itr))
				finalStates.push_back(i);
	}

	for (Map<Pair<Set<uint32_t>, char>, Set<uint32_t>>::const_iterator itr = TransitionFunction.begin(); itr != TransitionFunction.end(); itr++)
	{
		uint32_t currentState, nextState;

		for (uint32_t i = 0; i < States.size(); i++)
		{
			if (States[i] == itr->first.first)
				currentState = i;

			if (States[i] == itr->second)
				nextState = i;
		}

		Vector<uint32_t> transitionState;
		transitionState.push_back(nextState);
		transitionFunction.emplace(TransitionPair(currentState, itr->first.second), transitionState);
	}

	return DFA(states, initialState, finalStates, transitionFunction);
}

Set<uint32_t> NondeterministicFiniteAutomata::lambdaClosure(const uint32_t& state) const
{
	if (state >= _states)
		return Set<uint32_t>();

	Set<uint32_t> closure;
	Queue<uint32_t> queue;
	Vector<bool> visited(_states, false);

	queue.push(state);
	closure.insert(state);
	visited[state] = true;

	while (!queue.empty())
	{
		uint32_t currentState = queue.front();
		bool found = false;

		TransitionMap::const_iterator itr = _transitionFunction.find(TransitionPair(currentState, '0'));

		if (itr != _transitionFunction.end())
		{
			for (Vector<uint32_t>::const_iterator _itr = itr->second.begin(); _itr != itr->second.end(); _itr++)
			{
				if (!visited[(*_itr)])
				{
					closure.insert((*_itr));
					queue.push((*_itr));
					visited[(*_itr)] = true;
				}
			}
		}

		queue.pop();
	}

	return closure;
}

Set<uint32_t> NondeterministicFiniteAutomata::lambdaClosure(const Set<uint32_t>& states) const
{
	if (states.empty())
		return Set<uint32_t>();

	Set<uint32_t> closure;

	for (Set<uint32_t>::const_iterator itr = states.begin(); itr != states.end(); itr++)
	{
		Set<uint32_t> _closure = lambdaClosure(*itr);
		std::set_union(_closure.begin(), _closure.end(), closure.begin(), closure.end(), std::inserter(closure, closure.begin()));
	}

	return closure;
}

Set<uint32_t> NondeterministicFiniteAutomata::moveTo(const uint32_t& state, const char& key) const
{
	if (state >= _states)
		return Set<uint32_t>();

	TransitionMap::const_iterator itr = _transitionFunction.find(TransitionPair(state, key));

	if (itr != _transitionFunction.end())
	{
		Set<uint32_t> closure;

		// Should _transitionFunction have value of type set instead of vector?
		for (Vector<uint32_t>::const_iterator _itr = itr->second.begin(); _itr != itr->second.end(); _itr++)
			closure.insert(*_itr);

		return closure;
	}

	return Set<uint32_t>();
}

Set<uint32_t> NondeterministicFiniteAutomata::moveTo(const Set<uint32_t>& states, const char& key) const
{
	if (states.empty())
		return Set<uint32_t>();

	Set<uint32_t> closure;

	for (Set<uint32_t>::const_iterator itr = states.begin(); itr != states.end(); itr++)
	{
		Set<uint32_t> _closure = moveTo(*itr, key);
		std::set_union(_closure.begin(), _closure.end(), closure.begin(), closure.end(), inserter(closure, closure.begin()));
	}

	return closure;
}

Set<char> NondeterministicFiniteAutomata::getAlphabet() const
{
	if (!_states || _transitionFunction.empty())
		return Set<char>();

	Set<char> alphabet;

	for (TransitionMap::const_iterator itr = _transitionFunction.begin(); itr != _transitionFunction.end(); itr++)
		if (itr->first.second != '0')
			alphabet.insert(itr->first.second);

	return alphabet;
}

