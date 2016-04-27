#include "PCH.h"
#include "FiniteAutomata.h"

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
	if (state > _states - 1 || !HasStates() || !HasFinalStates())
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

