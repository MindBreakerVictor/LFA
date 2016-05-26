#include "PCH.h"
#include "NondeterministicFiniteAutomata.h"

NondeterministicFiniteAutomata::NondeterministicFiniteAutomata(std::ifstream& ifs)
{
	uint32 finalStates;

	ifs >> _states >> _initialState >> finalStates;

	assert(_states >= 0 && (_initialState >= 0 && _initialState < _states) 
		&& (finalStates >= 0 && finalStates <= _states));

	for (uint32 i = 0; i < finalStates; ++i)
	{
		uint32 finalState;

		ifs >> finalState;
		_finalStates.push_back(finalState);
	}

	while (!ifs.eof())
	{
		char key;
		uint32 currentState, nextState;

		ifs >> currentState >> key >> nextState;

		TransitionPair pair(currentState, key);
		TransitionMap::iterator itr = _transitionFunction.find(pair);

		if (itr == _transitionFunction.end())
			_transitionFunction.emplace(pair, StatesVector({ nextState }));
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

String NondeterministicFiniteAutomata::GenerateWord(uint32 const& length) const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates() || !length)
		return String();

	return ToDFA().GenerateWord(length);
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
	for (uint32 i = 0; i < States.size(); ++i)
	{
		for (Set<char>::const_iterator key = alphabet.begin(); key != alphabet.end(); ++key)
		{
			StatesSet _state = LambdaClosure(MoveTo(States[i], *key));

			if (!_state.empty())
			{
				if (std::find(States.begin(), States.end(), _state) == States.end())
					States.push_back(_state);

				TransitionFunction[Pair<StatesSet, char>(States[i], *key)] = _state;
			}
		}
	}

	// Set final states in the subset version of the DFA.
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
	uint32 states = static_cast<uint32>(States.size()), initialState = 0;
	StatesVector finalStates;
	TransitionMap transitionFunction;

	for (Vector<StatesSet>::const_iterator itr = FinalStates.begin(); itr != FinalStates.end(); ++itr)
	{
		for (uint32 i = 0; i < States.size(); ++i)
			if (States[i] == (*itr))
				finalStates.push_back(i);
	}

	for (Map<Pair<StatesSet, char>, StatesSet>::const_iterator itr = TransitionFunction.begin(); itr != TransitionFunction.end(); ++itr)
	{
		uint8 found = 0;
		uint32 currentState = 0, nextState = 0;	// Initialize them to remove warning.

		for (uint32 i = 0; i < States.size() && found != 2; ++i)
		{
			if (States[i] == itr->first.first)
			{
				currentState = i;
				++found;
			}

			if (States[i] == itr->second)
			{
				nextState = i;
				++found;
			}
		}

		if (found == 2)
			transitionFunction.emplace(TransitionPair(currentState, itr->first.second), StatesVector({ nextState }));
	}

	return DFA(states, initialState, finalStates, transitionFunction);
}

StatesSet NondeterministicFiniteAutomata::LambdaClosure(uint32 const& state) const
{
	StatesSet closure;
	Queue<uint32> queue;
	Vector<bool> visited(_states, false);

	queue.push(state);
	closure.insert(state);
	visited[state] = true;

	while (!queue.empty())
	{
		uint32 currentState = queue.front();

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

StatesSet NondeterministicFiniteAutomata::MoveTo(uint32 const& state, char const& key) const
{
	TransitionMapConstIterator itr = _transitionFunction.find(TransitionPair(state, key));

	if (itr != _transitionFunction.end())
	{
		StatesSet closure;

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

	for (StatesSetConstIterator itr = states.begin(); itr != states.end(); ++itr)
	{
		StatesSet _closure = MoveTo(*itr, key);
		std::set_union(_closure.begin(), _closure.end(), closure.begin(), closure.end(), inserter(closure, closure.begin()));
	}

	return closure;
}

