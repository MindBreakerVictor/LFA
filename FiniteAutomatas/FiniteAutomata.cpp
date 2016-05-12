#include "PCH.h"
#include "FiniteAutomata.h"
#include "NondeterministicFiniteAutomata.h"

void FiniteAutomata::RemoveState(uint32_t const& state)
{
	if (!HasStates())
		return;

	// Remove it from finalStates vector if it is a final state
	for (StatesConstIterator itr = _finalStates.begin(); itr != _finalStates.end(); ++itr)
		if ((*itr) == state)
		{
			_finalStates.erase(itr);
			break;
		}

	// Remove all its references from transition function
	TransitionMap::iterator itr = _transitionFunction.begin();
	while (itr != _transitionFunction.end())
	{
		if (itr->first.first == state)
		{
			_transitionFunction.erase(itr++);
			continue;
		}

		bool emptyTransition = false;

		for (StatesConstIterator iter = itr->second.begin(); iter != itr->second.end(); ++iter)
			if ((*iter) == state)
			{
				itr->second.erase(iter);

				if (itr->second.empty())
					emptyTransition = true;

				break;
			}

		if (emptyTransition)
		{
			_transitionFunction.erase(itr++);
			continue;
		}

		++itr;
	}

	_states--;
}

void FiniteAutomata::RemoveUnreachableStates()
{
	if (!HasStates())
		return;

	if (!HasTransitions())
	{
		_states = 1;	// Only the initial state is reachable.

		if (IsFinalState(_initialState))
		{
			_finalStates.~vector();
			_finalStates.push_back(_initialState);
		}
		else
			_finalStates.~vector();
	}

	Vector<bool> reachableStates = GetReachableStates();

	for (uint32_t i = 0; i < reachableStates.size(); ++i)
		if (!reachableStates[i])
			RemoveState(i);
}

void FiniteAutomata::Minimize()
{
	// This methods is using Brzozowski's algorithm.
	// Reverses a FA(either deterministic or nondeterministic)
	// then converts it to a DFA and then reverses it again.
	// This will a produce a minimal DFA for sure.
	// It can be used for NFA even if it produces a DFA.
	// NFAs are extensions of DFAs.
	*this = GetReverse().ToDFA().GetReverse().ToDFA();
}

Vector<bool> FiniteAutomata::GetReachableStates() const
{
	if (!HasStates())
		return Vector<bool>();

	Stack<uint32_t> stack;
	Vector<bool> visited(_states, false);
	
	stack.push(_initialState);
	visited[_initialState] = true;

	while (!stack.empty())
	{
		uint32_t currentState = stack.top();
		bool found = false;

		for (TransitionMapConstIterator itr = _transitionFunction.begin(); itr != _transitionFunction.end() && !found; ++itr)
			if (itr->first.first == currentState)
				for (StatesConstIterator iter = itr->second.begin(); iter != itr->second.end(); ++iter)
					if (!visited[(*iter)])
					{
						stack.push((*iter));
						visited[(*iter)] = true;
						found = true;
						break;
					}

		if (!found)
			stack.pop();
	}

	return visited;
}

bool FiniteAutomata::IsFinalState(uint32_t const& state) const
{
	if (!HasStates() || !HasFinalStates())
		return false;

	for (StatesConstIterator itr = _finalStates.begin(); itr != _finalStates.end(); ++itr)
		if ((*itr) == state)
			return true;

	return false;
}

bool FiniteAutomata::IsFinalState(StatesSet const& state) const
{
	if (!HasStates() || !HasFinalStates() || state.empty())
		return false;

	for (StatesSetConstIterator itr = state.begin(); itr != state.end(); ++itr)
		if (IsFinalState(*itr))
			return true;

	return false;
}

NFA FiniteAutomata::GetReverse() const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return NFA();

	uint32_t initialState;
	States finalStates;
	TransitionMap transitionFunction;

	// Reverse transitions
	for (TransitionMapConstIterator itr = _transitionFunction.begin(); itr != _transitionFunction.end(); ++itr)
	{
		for (StatesConstIterator iter = itr->second.begin(); iter != itr->second.end(); ++iter)
		{
			TransitionPair pair(*iter, itr->first.second);
			TransitionMap::iterator iterator = transitionFunction.find(pair);

			if (iterator == transitionFunction.end())
			{
				States transitionStates;
				transitionStates.push_back(itr->first.first);
				transitionFunction.emplace(pair, transitionStates);
			}
			else
				iterator->second.push_back(itr->first.first);
		}
	}

	// Build initial state
	if (_finalStates.size() > 1)
	{
		initialState = _states;
		transitionFunction.emplace(std::make_pair(std::make_pair(_states, '0'), _finalStates));
	}
	else
		initialState = _finalStates[0];

	// Build final state
	finalStates.push_back(_initialState);

	// Build the reversed nfa
	return NFA((_finalStates.size() > 1) ? _states + 1 : _states, initialState, finalStates, transitionFunction);
}

FiniteAutomata& FiniteAutomata::operator=(FiniteAutomata const& source)
{
	if (this == &source)
		return *this;

	_states = source._states;
	_finalStates = source._finalStates;
	_initialState = source._initialState;
	_transitionFunction = source._transitionFunction;

	return *this;
}

