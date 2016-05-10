#ifndef _DETERMINISTIC_FINITE_AUTOMATA_H
#define _DETERMINISTIC_FINITE_AUTOMATA_H

#include "PCH.h"
#include "FiniteAutomata.h"
#include "RegularExpression.h"

class DeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		DeterministicFiniteAutomata() : FiniteAutomata() { }
		DeterministicFiniteAutomata(std::ifstream& ifs);
		DeterministicFiniteAutomata(DeterministicFiniteAutomata const& source) : FiniteAutomata(source) { }
		DeterministicFiniteAutomata(uint32_t const& states, uint32_t const& initialState, 
			States const& finalStates, TransitionMap const& transitionFunction) : 
			FiniteAutomata(states, initialState, finalStates, transitionFunction) { }

		void Reverse() override;

		bool IsAccepted(String const& word) const override;

		String GenerateWord(uint32_t const& length) const override;
		String GetRegularExpression() const override;

		Vector<Vector<String>> GetCoefficientsMatrix() const;
		Vector<Vector<String>> GetFreeTermsMatrix() const;

	private:
		bool GenerateWord(uint32_t const& currentState, uint32_t length, String* word) const;

		// Used in GetRegularExpression
		void ApplyArdensLemma(uint32_t const& index, 
			Vector<Vector<String>>* coefficientsMatrix,	Vector<Vector<String>>* freeTermsMatrix) const;
		void ApplyArdensLemma(String const& regex, String* freeTerm) const;
		void EliminateState(uint32_t const& index, 
			Vector<Vector<String>>* coefficientsMatrix,	Vector<Vector<String>>* freeTermsMatrix) const;
};

typedef DeterministicFiniteAutomata DFA;

#endif

