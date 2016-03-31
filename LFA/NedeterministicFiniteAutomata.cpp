#include "NedeterministicFiniteAutomata.h"

NedeterministicFiniteAutomata::NedeterministicFiniteAutomata(std::ifstream& ifs)
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
		_transitionTabel[state1].emplace(trans, state2);
	}
}

bool NedeterministicFiniteAutomata::isAccepted(const uint& state, const String& word) const
{
	if (word.empty())
	{
		if (isFinalState(state))
			return true;
		
		std::vector<uint> lambdaStates = lambdaClose(state);

		std::vector<uint>::const_iterator i;
		for (i = lambdaStates.begin(); i != lambdaStates.end(); i++)
			if (isFinalState(*i))
				return true;

		return false;
	}

	std::multimap<char, uint>::const_iterator iterator;
	for (iterator = _transitionTabel[state].begin(); iterator != _transitionTabel[state].end(); iterator++)
	{
		// There is a transition with the first char of word
		// or there is a lambda transition, from current state.
		if (iterator->first == word[0] || iterator->first == '0')
			if (isAccepted(iterator->second, std::string(word, 1)))
				return true;
	}

	return false;
}

std::vector<bool> NedeterministicFiniteAutomata::getReachableStates() const
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

		std::multimap<char, uint>::const_iterator iterator;
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

String NedeterministicFiniteAutomata::generateWord(const uint& length) const
{
	if (!_states || _finalStates.empty() || !length)
		return String();

	String word;
	generateWord(_initialState, length, word);

	if (word.length() == length)
		return word;

	return String();
}

void NedeterministicFiniteAutomata::generateWord(const uint& state, const uint& length, String& word) const
{
	if (!length)
		return;

	std::multimap<char, uint>::const_iterator iterator;
	for (iterator = _transitionTabel[state].begin(); iterator != _transitionTabel[state].end(); iterator++)
	{
		word += iterator->first;
		generateWord(iterator->second, length - 1, word);
		if (isAccepted(word))
			return;
		word.pop_back();
	}
}

std::vector<uint> NedeterministicFiniteAutomata::lambdaClose(uint state) const
{
	if (state < 0 || state > _states - 1)
		return std::vector<uint>();

	std::queue<uint> queue;
	std::vector<uint> close;
	queue.push(state);

	while (!queue.empty())
	{
		uint currentState = queue.front();
		bool found = false;

		std::multimap<char, uint>::const_iterator i = _transitionTabel[currentState].find('0');
		while (i != _transitionTabel[currentState].end() && i->first == '0')
		{
			std::vector<uint>::const_iterator it;
			for (it = close.begin(); it != close.end() && !found; it++)
				if (*it == i->second)
					found = true;

			if (!found)
			{
				close.push_back(i->second);
				queue.push(i->second);
			}

			i++;
		}

		queue.pop();
	}

	return close;
}

std::vector<uint> NedeterministicFiniteAutomata::lambdaClose(std::vector<uint> stateSet) const
{
	std::vector<uint> close;

	for (uint i = 0; i < stateSet.size(); i++)
	{
		std::vector<uint> stateLambdaClose = lambdaClose(stateSet[i]);
		
		for (uint j = 0; j < stateLambdaClose.size(); j++)
		{
			bool found = false;

			std::vector<uint>::const_iterator i;
			for (i = close.begin(); i != close.end(); i++)
				if (*i == stateLambdaClose[j])
					found = true;
			if (!found)
				close.push_back(stateLambdaClose[j]);
		}
	}

	return close;
}

std::vector<uint> NedeterministicFiniteAutomata::move(uint state, const char& key) const
{
	if (state < 0 || state > _states - 1)
		return std::vector<uint>();

	std::multimap<char, uint>::const_iterator mIt = _transitionTabel[state].find(key);

	if (mIt == _transitionTabel[state].end())
		return std::vector<uint>();

	std::vector<uint> keyClose;
	std::vector<uint>::const_iterator vIt;
	while ((mIt != _transitionTabel[state].end()) && (mIt->first == key))
	{
		bool found = false;
		
		for (vIt = keyClose.begin(); vIt != keyClose.end(); vIt++)
			if (*vIt == mIt->second)
				found = true;

		if (!found)
			keyClose.push_back(mIt->second);
		
		mIt++;
	}

	return keyClose;
}

std::vector<uint> NedeterministicFiniteAutomata::move(std::vector<uint> stateSet, const char& key) const
{
	std::vector<uint> keyClose;

	for (uint i = 0; i < stateSet.size(); i++)
	{
		std::vector<uint> stateKeyClose = move(stateSet[i], key);

		for (uint j = 0; j < stateKeyClose.size(); j++)
			keyClose.push_back(stateKeyClose[j]);
	}

	return keyClose;
}
