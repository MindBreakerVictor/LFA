#ifndef LFA_LIB_DETERMINISTIC_FINITE_AUTOMATA_H
#define LFA_LIB_DETERMINISTIC_FINITE_AUTOMATA_H

#include "PCH.h"
#include "FiniteAutomata.h"
#include "RegularExpression.h"

class DeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		DeterministicFiniteAutomata() : FiniteAutomata() { }
		DeterministicFiniteAutomata(std::ifstream& ifs);
		DeterministicFiniteAutomata(DeterministicFiniteAutomata const& source) : FiniteAutomata(source) { }
		DeterministicFiniteAutomata(uint32 const& states, uint32 const& initialState, 
			StatesVector const& finalStates, TransitionMap const& transitionFunction) : 
			FiniteAutomata(states, initialState, finalStates, transitionFunction) { }

		void Reverse() override;
		
		void Minimize(bool usingHopcroft = true);

		bool IsAccepted(String const& word) const override;

		String GenerateWord(uint32 const& length) const override;
		String GetRegularExpression() const;

		Vector<Vector<bool>> GetEquivalenceMatrix() const;

		Vector<Vector<String>> GetCoefficientsMatrix() const;
		Vector<Vector<String>> GetFreeTermsMatrix() const;

	private:
		bool GenerateWord(uint32 const& currentState, uint32 length, String* word) const;

		// Used in GetRegularExpression
		void ApplyArdensLemma(uint32 const& index, 
			Vector<Vector<String>>* coefficientsMatrix,	Vector<Vector<String>>* freeTermsMatrix) const;
		void ApplyArdensLemma(String const& regex, String* freeTerm) const;
		void EliminateState(uint32 const& index, 
			Vector<Vector<String>>* coefficientsMatrix,	Vector<Vector<String>>* freeTermsMatrix) const;

		// Used in Minimize
		Vector<Pair<StatesSet, bool>> BuildMooreMinimalStates() const;	// StatesVector are build using Moore's algorithm in O(N^2) time.
		Vector<Pair<StatesSet, bool>> BuildHopcroftMinimalStates() const;	// StatesVector are build using Hopcroft's algorithm in O(NlogN) time.

		StatesSet GetPredeccesors(StatesSet const& statesSet, char const& key) const;

		Pair<StatesSet, bool> MoveTo(Vector<Pair<StatesSet, bool>> const& powerSetStates, 
			Pair<StatesSet, bool> const& states, char const& key) const;
};

typedef DeterministicFiniteAutomata DFA;

#endif

