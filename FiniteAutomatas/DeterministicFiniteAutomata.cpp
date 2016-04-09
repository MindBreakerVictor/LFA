#include "PCH.h"
#include "DeterministicFiniteAutomata.h"

DeterministicFiniteAutomata::DeterministicFiniteAutomata(std::ifstream& ifs)
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
		Vector<uint32_t> transitionState;

		ifs >> currentState >> key >> nextState;
		
		// Check for lambda transition
		assert(key != '0');

		transitionState.push_back(nextState);
		_transitionFunction.emplace(TransitionPair(currentState, key), transitionState);
	}
}

bool DeterministicFiniteAutomata::isAccepted(const String& word) const
{
	if (!_states || _finalStates.empty() || _transitionFunction.empty())
		return false;

	uint32_t currentState = _initialState;

	for (String::const_iterator itr = word.begin(); itr != word.end(); itr++)
	{
		TransitionMap::const_iterator _itr = _transitionFunction.find(TransitionPair(currentState, *itr));

		if (_itr == _transitionFunction.end())
			return false;

		currentState = _itr->second.front();
	}

	if (!isFinalState(currentState))
		return false;

	return true;
}


String DeterministicFiniteAutomata::generateWord(const uint32_t& length) const
{
	if (!_states || _transitionFunction.empty() || _finalStates.empty() || !length)
		return String();

	String word;
	bool found = generateWord(_initialState, length, word);

	if (!found)
		return String(); 
	
	return word;	
}

bool DeterministicFiniteAutomata::generateWord(uint32_t currentState, uint32_t length, String& word) const
{
	if (!length && isFinalState(currentState))
		return true;

	for (TransitionMap::const_iterator itr = _transitionFunction.begin(); itr != _transitionFunction.end(); itr++)
	{
		if (itr->first.first == currentState)
		{
			word += itr->first.second;

			for (Vector<uint32_t>::const_iterator _itr = itr->second.begin(); _itr != itr->second.end(); _itr++)
			{
				bool found = generateWord(*_itr, length - 1, word);

				if (!found)
					word.pop_back();
				else
					return true;
			}
		}
	}

	return false;
}

