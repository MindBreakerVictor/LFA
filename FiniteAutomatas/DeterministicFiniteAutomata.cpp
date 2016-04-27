#include "PCH.h"
#include "DeterministicFiniteAutomata.h"

namespace
{
	Vector<Vector<String>> operator+(Vector<Vector<String>> const& first, Vector<Vector<String>> const& second)
	{
		assert(first.size() == second.size());
		for (uint32_t i = 0; i < first.size(); i++)
			assert(first[i].size() == second[i].size());

		Vector<Vector<String>> sum(first.size());

		for (uint32_t i = 0; i < first.size(); i++)
		{
			sum[i].resize(first[i].size(), "");

			for (uint32_t j = 0; j < first[i].size(); j++)
			{
				if (!first[i][j].empty() && !second[i][j].empty())
					sum[i][j] = "(" + first[i][j] + "+" + second[i][j] + ")";
				else if (first[i][j].empty() && !second[i][j].empty())
					sum[i][j] = second[i][j];
				else if (!first[i][j].empty() && second[i][j].empty())
					sum[i][j] = first[i][j];
			}
		}

		return sum;
	}

	Vector<Vector<String>>& operator+=(Vector<Vector<String>>& vectorOne, Vector<Vector<String>> const& vectorTwo)
	{
		vectorOne = vectorOne + vectorTwo;

		return vectorOne;
	}
}

DeterministicFiniteAutomata::DeterministicFiniteAutomata(std::ifstream& ifs)
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
		States transitionState;

		ifs >> currentState >> key >> nextState;
		
		// Check for lambda transition
		assert(key != '0');

		transitionState.push_back(nextState);
		_transitionFunction.emplace(TransitionPair(currentState, key), transitionState);
	}
}

bool DeterministicFiniteAutomata::IsAccepted(String const& word) const
{
	if (!HasStates() || !HasTransition() || !HasFinalStates())
		return false;

	uint32_t currentState = _initialState;

	for (String::const_iterator itr = word.begin(); itr != word.end(); ++itr)
	{
		TransitionMapConstIterator iter = _transitionFunction.find(TransitionPair(currentState, *itr));

		if (iter == _transitionFunction.end())
			return false;

		currentState = iter->second.front();
	}

	return IsFinalState(currentState);
}


String DeterministicFiniteAutomata::GenerateWord(uint32_t const& length) const
{
	if (!HasStates() || !HasTransition() || !HasFinalStates() || !length)
		return String();

	String word;
	bool found = GenerateWord(_initialState, length, &word);

	if (!found)
		return String(); 
	
	return word;
}

String DeterministicFiniteAutomata::GetRegularExpression() const
{
	if (!HasStates() || !HasTransition() || !HasFinalStates())
		return String();

	Vector<Vector<String>> coefficientsMatrix = GetCoefficientsMatrix();
	Vector<Vector<String>> freeTermsMatrix = GetFreeTermsMatrix();
	
	for (int i = _states - 1; i >= 0; --i)
	{
		Vector<Vector<String>> _A(_states, Vector<String>(_states, ""));
		Vector<Vector<String>> _B(_states, Vector<String>(1, ""));

		ApplyArdensLemma(i, &coefficientsMatrix, &freeTermsMatrix);
		EliminateState(i, &coefficientsMatrix, &freeTermsMatrix);
	}

	return freeTermsMatrix[0][0];
}

Vector<Vector<String>> DeterministicFiniteAutomata::GetCoefficientsMatrix() const
{
	Vector<Vector<String>> coefficientsMatrix(_states, Vector<String>(_states, ""));

	for (TransitionMapConstIterator itr = _transitionFunction.begin(); itr != _transitionFunction.end(); ++itr)
		coefficientsMatrix[itr->first.first][itr->second.at(0)] = itr->first.second;

	return coefficientsMatrix;
}

Vector<Vector<String>> DeterministicFiniteAutomata::GetFreeTermsMatrix() const
{
	Vector<Vector<String>> freeTermsMatrix(_states, Vector<String>(1, ""));

	for (uint32_t i = 0; i < _states; ++i)
		if (IsFinalState(i))
			freeTermsMatrix[i][0] = '0';

	return freeTermsMatrix;
}

bool DeterministicFiniteAutomata::GenerateWord(uint32_t const& currentState, uint32_t length, String* word) const
{
	if (!length && IsFinalState(currentState))
		return true;

	for (TransitionMapConstIterator itr = _transitionFunction.begin(); itr != _transitionFunction.end(); ++itr)
	{
		if (itr->first.first == currentState)
		{
			*word += itr->first.second;

			for (StatesConstIterator iter = itr->second.begin(); iter != itr->second.end(); ++iter)
			{
				bool found = GenerateWord(*iter, length - 1, word);

				if (!found)
					word->pop_back();
				else
					return true;
			}
		}
	}

	return false;
}

void DeterministicFiniteAutomata::ApplyArdensLemma(uint32_t const& index, 
	Vector<Vector<String>>* coefficientsMatrix, Vector<Vector<String>>* freeTermsMatrix) const
{
	String& regex = (*coefficientsMatrix)[index][index];

	if (!regex.empty())
	{
		ApplyArdensLemma(regex, &(*freeTermsMatrix)[index][0]);

		for (uint32_t i = 0; i < _states - 1; ++i)
			ApplyArdensLemma(regex, &(*coefficientsMatrix)[index][i]);

		regex.clear();
	}
}

void DeterministicFiniteAutomata::ApplyArdensLemma(String const& regex, String* freeTerm) const
{
	using namespace RegularExpression;

	if (freeTerm->empty())
		return;

	if ((*freeTerm) != "0")
	{
		*freeTerm = ((regex.size() > 1) && !IsInParentheses(regex)) ? Star(Parenthesize(regex)) + (*freeTerm) : Star(regex) + (*freeTerm);
		return;
	}

	*freeTerm = ((regex.size() > 1) && !IsInParentheses(regex)) ? Star(Parenthesize(regex)) : Star(regex);
}

void DeterministicFiniteAutomata::EliminateState(uint32_t const& index, 
	Vector<Vector<String>>* coefficientsMatrix, Vector<Vector<String>>* freeTermsMatrix) const
{
	Vector<Vector<String>> _A(_states, Vector<String>(_states, ""));
	Vector<Vector<String>> _B(_states, Vector<String>(1, ""));

	for (uint32_t i = 0; i < _states; ++i)
		if (!(*coefficientsMatrix)[i][index].empty())
			for (uint32_t j = 0; j < _states; ++j)
				if (!(*coefficientsMatrix)[index][j].empty())
					_A[i][j] = (*coefficientsMatrix)[i][index] + (*coefficientsMatrix)[index][j];

	(*coefficientsMatrix) += _A;

	if (!(*freeTermsMatrix)[index][0].empty())
		for (uint32_t i = 0; i < _states; ++i)
			if (!(*coefficientsMatrix)[i][index].empty())
			{
				_B[i][0] = (*coefficientsMatrix)[i][index];

				if ((*freeTermsMatrix)[index][0] != "0")
					_B[i][0] += (*freeTermsMatrix)[index][0];
			}

	(*freeTermsMatrix) += _B;
}

