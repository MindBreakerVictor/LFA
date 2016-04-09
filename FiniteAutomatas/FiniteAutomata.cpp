#include "PCH.h"
#include "FiniteAutomata.h"

Vector<bool> FiniteAutomata::getReachableStates() const
{
	if (!_states)
		return Vector<bool>();

	Stack<uint32_t> stack;
	Vector<bool> visited(_states, false);
	
	stack.push(_initialState);
	visited[_initialState] = true;

	while (!stack.empty())
	{
		uint32_t currentState = stack.top();
		bool found = false;

		TransitionMap::const_iterator itr;
		for (itr = _transitionFunction.begin(); itr != _transitionFunction.end() && !found; itr++)
			if (itr->first.first == currentState)
				for (Vector<uint32_t>::const_iterator _itr = itr->second.begin(); _itr != itr->second.end(); _itr++)
					if (!visited[(*_itr)])
					{
						stack.push((*_itr));
						visited[(*_itr)] = true;
						found = true;
						break;
					}

		if (!found)
			stack.pop();
	}

	return visited;
}

bool FiniteAutomata::isFinalState(const uint32_t& state) const
{
	if (state > _states - 1 || !_states || _finalStates.empty())
		return false;

	Vector<uint32_t>::const_iterator itr;
	for (itr = _finalStates.begin(); itr != _finalStates.end(); itr++)
		if ((*itr) == state)
			return true;

	return false;
}

bool FiniteAutomata::isFinalState(const Set<uint32_t>& state) const
{
	if (!_states || _finalStates.empty() || state.empty())
		return false;

	Set<uint32_t>::const_iterator itr;
	for (itr = state.begin(); itr != state.end(); itr++)
		if (isFinalState(*itr))
			return true;

	return false;
}

