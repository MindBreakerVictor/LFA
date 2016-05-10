#include "PCH.h"
#include "NondeterministicFiniteAutomata.h"

NondeterministicFiniteAutomata::NondeterministicFiniteAutomata(std::ifstream& ifs)
{
	uint32_t finalStates;

	ifs >> _states >> _initialState >> finalStates;

	assert(_states >= 0 && (_initialState >= 0 && _initialState < _states) 
		&& (finalStates >= 0 && finalStates <= _states));

	for (uint32_t i = 0; i < finalStates; ++i)
	{
		uint32_t finalState;

		ifs >> finalState;
		_finalStates.push_back(finalState);
	}

	while (!ifs.eof())
	{
		char key;
		uint32_t currentState, nextState;

		ifs >> currentState >> key >> nextState;

		TransitionPair pair(currentState, key);
		TransitionMap::iterator itr = _transitionFunction.find(pair);

		if (itr == _transitionFunction.end())
		{
			States transitionStates;
			transitionStates.push_back(nextState);
			_transitionFunction.emplace(pair, transitionStates);
		}
		else
			itr->second.push_back(nextState);
	}
}

void NondeterministicFiniteAutomata::Reverse()
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return;

	NFA reversedNFA = GetReverse();

	_states = reversedNFA._states;
	_finalStates = reversedNFA._finalStates;
	_initialState = reversedNFA._initialState;
	_transitionFunction = reversedNFA._transitionFunction;
}

bool NondeterministicFiniteAutomata::IsAccepted(String const& word) const
{
	if (!HasStates() || !HasFinalStates() || !HasTransitions())
		return false;

	if (word.empty() && IsFinalState(LambdaClosure(_initialState)))
		return true;

	if (ToDFA().IsAccepted(word))
		return true;

	return false;
}

String NondeterministicFiniteAutomata::GenerateWord(uint32_t const& length) const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates() || !length)
		return String();

	return ToDFA().GenerateWord(length);
}

String NondeterministicFiniteAutomata::GetRegularExpression() const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return String();

	return ToDFA().GetRegularExpression();
}

DFA NondeterministicFiniteAutomata::ToDFA() const
{
	// We don't check for finalStates
	if (!HasStates() || !HasTransitions())
		return DFA();

	// Variables to hold the subset version of the DFA.
	Set<char> const alphabet = GetAlphabet();
	Vector<StatesSet> States, FinalStates;
	StatesSet InitialState(LambdaClosure(_initialState));
	Map<Pair<StatesSet, char>, StatesSet> TransitionFunction;
	States.push_back(InitialState);

	// Construct TransitionFunction using subset construction.
	// Cannot use iterator because we constantly add elements in States.
	for (uint32_t i = 0; i < States.size(); ++i)
	{
		for (Set<char>::const_iterator key = alphabet.begin(); key != alphabet.end(); ++key)
		{
			StatesSet _state = LambdaClosure(MoveTo(States[i], *key));

			if ((std::find(States.begin(), States.end(), _state) == States.end()) && !_state.empty())
				States.push_back(_state);

			if (!_state.empty())
				TransitionFunction[Pair<StatesSet, char>(States[i], *key)] = _state;
		}
	}

	// Set final states in the subset version fo the DFA.
	for (Vector<StatesSet>::const_iterator itr = States.begin(); itr != States.end(); ++itr)
	{
		for (StatesSetConstIterator iter = itr->begin(); iter != itr->end(); ++iter)
		{
			if (IsFinalState((*iter)))
			{
				FinalStates.push_back(*itr);
				break;
			}
		}
	}

	// Variables to hold the DFA.
	// The new DFA has his states indexed by their index in States.
	uint32_t states = States.size(), initialState = 0;
	Vector<uint32_t> finalStates;
	TransitionMap transitionFunction;

	for (Vector<StatesSet>::const_iterator itr = FinalStates.begin(); itr != FinalStates.end(); ++itr)
	{
		for (uint32_t i = 0; i < States.size(); ++i)
			if (States[i] == (*itr))
				finalStates.push_back(i);
	}

	for (Map<Pair<StatesSet, char>, StatesSet>::const_iterator itr = TransitionFunction.begin(); itr != TransitionFunction.end(); ++itr)
	{
		uint32_t currentState, nextState;

		for (uint32_t i = 0; i < States.size(); ++i)
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

StatesSet NondeterministicFiniteAutomata::LambdaClosure(uint32_t const& state) const
{
	StatesSet closure;
	Queue<uint32_t> queue;
	Vector<bool> visited(_states, false);

	queue.push(state);
	closure.insert(state);
	visited[state] = true;

	while (!queue.empty())
	{
		uint32_t currentState = queue.front();

		TransitionMapConstIterator itr = _transitionFunction.find(TransitionPair(currentState, '0'));

		if (itr != _transitionFunction.end())
		{
			for (StatesConstIterator iter = itr->second.begin(); iter != itr->second.end(); ++iter)
			{
				if (!visited[(*iter)])
				{
					closure.insert((*iter));
					queue.push((*iter));
					visited[(*iter)] = true;
				}
			}
		}

		queue.pop();
	}

	return closure;
}

StatesSet NondeterministicFiniteAutomata::LambdaClosure(StatesSet const& states) const
{
	if (states.empty())
		return StatesSet();

	StatesSet closure;

	for (StatesSetConstIterator itr = states.begin(); itr != states.end(); ++itr)
	{
		StatesSet _closure = LambdaClosure(*itr);
		std::set_union(_closure.begin(), _closure.end(), closure.begin(), closure.end(), std::inserter(closure, closure.begin()));
	}

	return closure;
}

StatesSet NondeterministicFiniteAutomata::MoveTo(uint32_t const& state, char const& key) const
{
	TransitionMapConstIterator itr = _transitionFunction.find(TransitionPair(state, key));

	if (itr != _transitionFunction.end())
	{
		StatesSet closure;

		// Should _transitionFunction have value of type set instead of vector?
		for (StatesConstIterator iter = itr->second.begin(); iter != itr->second.end(); ++iter)
			closure.insert(*iter);

		return closure;
	}

	return StatesSet();
}

StatesSet NondeterministicFiniteAutomata::MoveTo(StatesSet const& states, char const& key) const
{
	if (states.empty())
		return StatesSet();

	StatesSet closure;

	for (StatesSetConstIterator itr = states.begin(); itr != states.end(); itr++)
	{
		StatesSet _closure = MoveTo(*itr, key);
		std::set_union(_closure.begin(), _closure.end(), closure.begin(), closure.end(), inserter(closure, closure.begin()));
	}

	return closure;
}

Set<char> NondeterministicFiniteAutomata::GetAlphabet() const
{
	if (!HasStates() || !HasTransitions())
		return Set<char>();

	Set<char> alphabet;

	for (TransitionMapConstIterator itr = _transitionFunction.begin(); itr != _transitionFunction.end(); itr++)
		if (itr->first.second != '0')
			alphabet.insert(itr->first.second);

	return alphabet;
}

