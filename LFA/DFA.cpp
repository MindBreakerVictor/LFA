#include "DFA.h"

DFA::DFA(std::ifstream& input)
{
	uint finalStatesSize;
	input >> states >> startState >> finalStatesSize;
	assert(finalStatesSize <= states);
	finalStates.resize(finalStatesSize);
	transitionTabel.resize(states);

	for (uint i = 0; i < finalStatesSize; i++)
		input >> finalStates[i];

	while (!input.eof())
	{
		uint state1, state2;
		char trans;
		input >> state1 >> trans >> state2;
		transitionTabel[state1].emplace(trans, state2);
	}
}

bool DFA::checkString(const std::string& string) const
{
	if (string.empty())
		return false;

	uint currentState = startState;

	for (uint i = 0; i < string.size(); i++)
	{
		// Iterator to find if there is a transition from currentState with the char from position i of string
		std::map<char, uint>::const_iterator iterator = transitionTabel[currentState].find(string.at(i));

		// There is no transition from currentState with the char from position i of string
		if (iterator == transitionTabel[currentState].end())
			return false;

		// Set the new currentState
		currentState = transitionTabel[currentState].at(string.at(i));
	}

	if (!isFinal(currentState))
		return false;

	return true;
}

std::vector<bool> DFA::getReachableStates() const
{
	if (!states || finalStates.empty())
		return std::vector<bool>();

	std::vector<bool> visited(states);
	std::stack<uint> stack;

	stack.push(0);
	visited[0] = true;

	while (!stack.empty())
	{
		uint element = stack.top();
		bool found = false;

		std::map<char, uint>::const_iterator iterator;
		for (iterator = transitionTabel[element].begin(); iterator != transitionTabel[element].end() && !found; iterator++)
			if (!visited[iterator->second])
				found = true;

		if (found)
		{
			iterator--;
			stack.push(iterator->second);
			visited[iterator->second] = true;
		}
		else
			stack.pop();
	}

	return visited;
}

std::string DFA::generateWord(const uint& lenght) const
{
	if (!states || finalStates.empty())
		return std::string();
}

bool DFA::isFinal(uint state) const
{
	for (uint i = 0; i < finalStates.size(); i++)
		if (finalStates[i] == state)
			return true;

	return false;
}
