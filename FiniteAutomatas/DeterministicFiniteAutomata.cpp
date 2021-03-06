#include "PCH.h"
#include "DeterministicFiniteAutomata.h"
#include "NondeterministicFiniteAutomata.h"

namespace
{
	Vector<Vector<String>> operator+(Vector<Vector<String>> const& first, Vector<Vector<String>> const& second)
	{
		assert(first.size() == second.size());
		for (uint32 i = 0; i < first.size(); i++)
			assert(first[i].size() == second[i].size());

		Vector<Vector<String>> sum(first.size());

		for (uint32 i = 0; i < first.size(); i++)
		{
			sum[i].resize(first[i].size(), "");

			for (uint32 j = 0; j < first[i].size(); j++)
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

	StatesSet SetMin(StatesSet const& setOne, StatesSet const& setTwo)
	{
		return (setOne.size() <= setTwo.size()) ? setOne : setTwo;
	}

	bool Refines(StatesSet const& Y, StatesSet const& X, StatesSet* YONE, StatesSet* YTWO)
	{
		if (Y.empty() || X.empty())
			return false;

		YONE->clear();
		YTWO->clear();

		for (StatesSetConstIterator itr = Y.cbegin(); itr != Y.cend(); ++itr)
		{
			if (X.find(*itr) != X.cend())
				YONE->insert(*itr);
			else
				YTWO->insert(*itr);
		}

		if (!YONE->empty() && !YTWO->empty())
			return true;

		return false;
	}
}

DeterministicFiniteAutomata::DeterministicFiniteAutomata(std::ifstream& ifs)
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
		
		// Check for lambda transition
		assert(key != '0');

		_transitionFunction.emplace(TransitionPair(currentState, key), StatesVector({ nextState }));
	}
}

void DeterministicFiniteAutomata::Reverse()
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return;

	DFA reversedDFA = GetReverse().ToDFA();

	_states = reversedDFA._states;
	_finalStates = reversedDFA._finalStates;
	_initialState = reversedDFA._initialState;
	_transitionFunction = reversedDFA._transitionFunction;
}

void DeterministicFiniteAutomata::Minimize(bool usingHopcroft)
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return;

	StatesVector finalStates;
	TransitionMap transitionFunction;
	Map<Pair<Pair<StatesSet, bool>, char>, Pair<StatesSet, bool>> TransitionFunction;

	Set<char> alphabet = GetAlphabet();
	Vector<Pair<StatesSet, bool>> powerSetStates = usingHopcroft ? BuildHopcroftMinimalStates() : BuildMooreMinimalStates();

	// Build transition function and final state 
	// for minimal dfa. StatesVector are power sets from 
	// the states of the dfa.
	for (uint32 i = 0; i < powerSetStates.size(); ++i)
	{
		for (Set<char>::const_iterator key = alphabet.begin(); key != alphabet.end(); ++key)
		{
			Pair<StatesSet, bool> state = MoveTo(powerSetStates, powerSetStates[i], *key);

			if (!state.first.empty())
				TransitionFunction[Pair<Pair<StatesSet, bool>, char>(powerSetStates[i], *key)] = state;
		}

		if (powerSetStates[i].second)
			finalStates.push_back(i);
	}

	// Convert the power set states in simple states.
	Map<Pair<Pair<StatesSet, bool>, char>, Pair<StatesSet, bool>>::const_iterator itr;
	for (itr = TransitionFunction.begin(); itr != TransitionFunction.end(); ++itr)
	{
		uint8 found = 0;
		uint32 currentState = 0, nextState = 0;	// Initialize them to remove warning.

		for (uint32 i = 0; i < powerSetStates.size() && found != 2; ++i)
		{
			if (powerSetStates[i] == itr->first.first)
			{
				currentState = i;
				++found;
			}

			if (powerSetStates[i] == itr->second)
			{
				nextState = i;
				++found;
			}
		}

		if (found == 2)
			transitionFunction.emplace(TransitionPair(currentState, itr->first.second), Vector<uint32>({ nextState }));
	}

	// Set dfa properties to minimal dfa properties.
	_transitionFunction = transitionFunction;
	_finalStates = finalStates;
	_states = static_cast<uint32>(powerSetStates.size());
}

bool DeterministicFiniteAutomata::IsAccepted(String const& word) const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return false;

	uint32 currentState = _initialState;

	for (String::const_iterator itr = word.begin(); itr != word.end(); ++itr)
	{
		TransitionMapConstIterator iter = _transitionFunction.find(TransitionPair(currentState, *itr));

		if (iter == _transitionFunction.end())
			return false;

		currentState = iter->second.front();
	}

	return IsFinalState(currentState);
}

String DeterministicFiniteAutomata::GenerateWord(uint32 const& length) const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates() || !length)
		return String();

	String word;
	bool found = GenerateWord(_initialState, length, &word);

	if (!found)
		return String(); 
	
	return word;
}

String DeterministicFiniteAutomata::GetRegularExpression() const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
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

Vector<Vector<bool>> DeterministicFiniteAutomata::GetEquivalenceMatrix() const
{
	if (!HasStates() || !HasTransitions() || !HasFinalStates())
		return Vector<Vector<bool>>();

	Vector<Vector<bool>> distinct(_states, Vector<bool>(_states, false));
	Vector<Pair<uint32, uint32>> visited;

	for (uint32 i = 0; i < _finalStates.size(); ++i)
	{
		for (uint32 j = 0; j < _finalStates[i]; ++j)
			if (!IsFinalState(j))
			{
				distinct[_finalStates[i]][j] = true;
				distinct[j][_finalStates[i]] = true;
				visited.push_back(std::make_pair(j, _finalStates[i]));
			}
	}

	Set<char> alphabet = GetAlphabet();

	for (uint32 i = 0; i < visited.size(); ++i)
	{
		for (Set<char>::const_iterator key = alphabet.begin(); key != alphabet.end(); ++key)
		{
			for (uint32 firstState = 0; firstState < _states; ++firstState)
			{
				TransitionMapConstIterator firstTransition = _transitionFunction.find(TransitionPair(firstState, *key));

				if (firstTransition == _transitionFunction.end())
					continue;

				if (firstTransition->second.front() != visited[i].first)
					continue;

				for (uint32 secondState = 0; secondState < _states; ++secondState)
				{
					TransitionMapConstIterator secondTransition = _transitionFunction.find(TransitionPair(secondState, *key));

					if (secondTransition == _transitionFunction.end())
						continue;

					if ((secondTransition->second.front() != visited[i].second) || distinct[firstState][secondState])
						continue;

					visited.push_back(std::make_pair(firstState, secondState));
					distinct[firstState][secondState] = true;
					distinct[secondState][firstState] = true;
				}
			}
		}
	}

	return distinct;
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

	for (uint32 i = 0; i < _states; ++i)
		if (IsFinalState(i))
			freeTermsMatrix[i][0] = '0';

	return freeTermsMatrix;
}

bool DeterministicFiniteAutomata::GenerateWord(uint32 const& currentState, uint32 length, String* word) const
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

void DeterministicFiniteAutomata::ApplyArdensLemma(uint32 const& index, 
	Vector<Vector<String>>* coefficientsMatrix, Vector<Vector<String>>* freeTermsMatrix) const
{
	String& regex = (*coefficientsMatrix)[index][index];

	if (!regex.empty())
	{
		ApplyArdensLemma(regex, &(*freeTermsMatrix)[index][0]);

		for (uint32 i = 0; i < _states - 1; ++i)
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

void DeterministicFiniteAutomata::EliminateState(uint32 const& index, 
	Vector<Vector<String>>* coefficientsMatrix, Vector<Vector<String>>* freeTermsMatrix) const
{
	Vector<Vector<String>> _A(_states, Vector<String>(_states, ""));
	Vector<Vector<String>> _B(_states, Vector<String>(1, ""));

	for (uint32 i = 0; i < _states; ++i)
		if (!(*coefficientsMatrix)[i][index].empty())
			for (uint32 j = 0; j < _states; ++j)
				if (!(*coefficientsMatrix)[index][j].empty())
					_A[i][j] = (*coefficientsMatrix)[i][index] + (*coefficientsMatrix)[index][j];

	(*coefficientsMatrix) += _A;

	if (!(*freeTermsMatrix)[index][0].empty())
		for (uint32 i = 0; i < _states; ++i)
			if (!(*coefficientsMatrix)[i][index].empty())
			{
				_B[i][0] = (*coefficientsMatrix)[i][index];

				if ((*freeTermsMatrix)[index][0] != "0")
					_B[i][0] += (*freeTermsMatrix)[index][0];
			}

	(*freeTermsMatrix) += _B;
}

Vector<Pair<StatesSet, bool>> DeterministicFiniteAutomata::BuildMooreMinimalStates() const
{
	Vector<bool> marked(_states, false);
	Vector<Pair<StatesSet, bool>> states;
	Vector<Vector<bool>> distinct = GetEquivalenceMatrix();

	for (uint32 i = 0; i < distinct.size(); ++i)
	{
		if (marked[i])
			continue;

		marked[i] = true;

		bool final = false;

		StatesSet powerSet = { i };

		if (IsFinalState(i))
			final = true;

		for (uint32 j = i + 1; j < distinct.size(); ++j)
			if (!distinct[j][i])
			{
				powerSet.insert(j);
				marked[j] = true;

				if (!final && IsFinalState(j))
					final = true;
			}

		states.push_back(std::make_pair(powerSet, final));
	}

	return states;
}

Vector<Pair<StatesSet, bool>> DeterministicFiniteAutomata::BuildHopcroftMinimalStates() const
{
	Set<char> alphabet = GetAlphabet();
	Vector<StatesSet> P = { GetInconclusiveStates(), GetFinalStates() };
	Vector<StatesSet> W;
	W.push_back(SetMin(P.front(), P.back()));

	while (!W.empty())
	{
		StatesSet A = W.front();
		W.erase(W.begin());

		for (Set<char>::const_iterator itr = alphabet.cbegin(); itr != alphabet.cend(); ++itr)
		{
			StatesSet predeccesors = GetPredeccesors(A, *itr);

			if (predeccesors.empty())
				continue;

			Vector<StatesSet>::const_iterator iter = P.cbegin();

			while (iter != P.cend())
			{
				StatesSet YONE, YTWO;

				if (!Refines(*iter, predeccesors, &YONE, &YTWO))
				{
					++iter;
					continue;
				}

				Vector<StatesSet>::const_iterator iterator = std::find(W.cbegin(), W.cend(), *iter);

				if (iterator != W.cend())
				{
					W.erase(iterator);
					W.push_back(YONE);
					W.push_back(YTWO);
				}
				else
					W.push_back(SetMin(YONE, YTWO));

				iter = P.insert(iter, YTWO);
				iter = P.insert(iter, YONE);
				iter = P.erase(iter + 2);
			}
		}
	}

	Vector<Pair<StatesSet, bool>> powerSetStates;

	for (Vector<StatesSet>::const_iterator itr = P.cbegin(); itr != P.cend(); ++itr)
	{
		bool final = false;

		for (StatesSet::const_iterator iter = itr->cbegin(); iter != itr->cend() && !final; ++iter)
			if (IsFinalState((*iter)))
				final = true;

		powerSetStates.emplace_back(*itr, final);
	}

	return powerSetStates;
}

StatesSet DeterministicFiniteAutomata::GetPredeccesors(StatesSet const& statesSet, char const& key) const
{
	if (!HasStates() || !HasTransitions())
		return StatesSet();

	StatesSet predeccesors;

	for (TransitionMapConstIterator itr = _transitionFunction.cbegin(); itr != _transitionFunction.cend(); ++itr)
	{
		if (itr->first.second != key)
			continue;

		for (uint32 i = 0; i < itr->second.size(); ++i)
		{
			if (statesSet.find(itr->second[i]) != statesSet.end())
				predeccesors.insert(itr->first.first);
		}
	}

	return predeccesors;
}

Pair<StatesSet, bool> DeterministicFiniteAutomata::MoveTo(Vector<Pair<StatesSet, bool>> const& powerSetStates, 
	Pair<StatesSet, bool> const& states, char const& key) const
{
	if (states.first.empty())
		return Pair<StatesSet, bool>();

	bool final = false;
	StatesSet closure;

	for (StatesSetConstIterator itr = states.first.begin(); itr != states.first.end(); ++itr)
	{
		TransitionMapConstIterator iter = _transitionFunction.find(TransitionPair(*itr, key));

		if (iter != _transitionFunction.end())
		{
			closure.insert(iter->second.front());

			if (!final && IsFinalState(iter->second.front()))
				final = true;
		}
	}

	if (closure.empty())
		return Pair<StatesSet, bool>();

	// Look for it in powerSetStates
	// closure is not guaranteed in powerSetStates
	// it may be just a power set of a state from powerSetStates.
	
	// It is in powerSetStates.
	Vector<Pair<StatesSet, bool>>::const_iterator itr;
	for (itr = powerSetStates.begin(); itr != powerSetStates.end(); ++itr)
		if (itr->first == closure)
			return Pair<StatesSet, bool>(closure, final);

	// closure is just a power set of state from powerSetStates.
	for (uint32 i = 0; i < powerSetStates.size(); ++i)
		if (std::find(powerSetStates[i].first.begin(), powerSetStates[i].first.end(), *(closure.begin())) != powerSetStates[i].first.end())
			return powerSetStates[i];

	return Pair<StatesSet, bool>();
}

