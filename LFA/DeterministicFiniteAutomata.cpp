#include "DeterministicFiniteAutomata.h"

DeterministicFiniteAutomata::DeterministicFiniteAutomata(std::ifstream& ifs)
{
	uint finalStatesSize;
	ifs >> _states >> _initialState >> finalStatesSize;
	assert(_initialState >= 0 && _initialState < _states);
	assert(finalStatesSize <= _states);
	_finalStates.resize(finalStatesSize);
	_transitionTabel.resize(_states);

	for (uint i = 0; i < finalStatesSize; i++)
		ifs >> _finalStates[i];

	while (!ifs.eof())
	{
		uint state1, state2;
		char trans;
		ifs >> state1 >> trans >> state2;
		assert(trans != '0');	// 0 is for lambda transition, thus used for a NFA
		_transitionTabel[state1].emplace(trans, state2);
	}
}

bool DeterministicFiniteAutomata::isAccepted(const String& string) const
{
	uint currentState = _initialState;

	for (uint i = 0; i < string.size(); i++)
	{
		// Iterator to find if there is a transition from currentState with the char from position i of string
		std::map<char, uint>::const_iterator iterator = _transitionTabel[currentState].find(string.at(i));

		// There is no transition from currentState with the char from position i of string
		if (iterator == _transitionTabel[currentState].end())
			return false;

		// Set the new currentState
		currentState = _transitionTabel[currentState].at(string.at(i));
	}

	if (!isFinalState(currentState))
		return false;

	return true;
}

std::vector<bool> DeterministicFiniteAutomata::getReachableStates() const
{
	if (!_states || _finalStates.empty())
		return std::vector<bool>();

	std::vector<bool> visited(_states);
	std::stack<uint> stack;

	stack.push(_initialState);
	visited[_initialState] = true;

	while (!stack.empty())
	{
		uint element = stack.top();
		bool found = false;

		std::map<char, uint>::const_iterator iterator;
		for (iterator = _transitionTabel[element].begin(); iterator != _transitionTabel[element].end() && !found; iterator++)
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

String DeterministicFiniteAutomata::generateWord(const uint& length) const
{
	if (!_states || _finalStates.empty() || !length)
		return std::string();

	std::string word;
	generateWord(_initialState, length, word);

	if (word.length() == length)
		return word;
	
	return std::string();
}

void DeterministicFiniteAutomata::generateWord(const uint& state, const uint& length, String& word) const
{
	if (!length)
		return;

	std::map<char, uint>::const_iterator iterator;
	for (iterator = _transitionTabel[state].begin(); iterator != _transitionTabel[state].end(); iterator++)
	{
		word += iterator->first;
		generateWord(iterator->second, length - 1, word);
		if (isAccepted(word))
			return;
		word.pop_back();
	}
}
