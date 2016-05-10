#ifndef _NONDETERMINISTIC_FINITE_AUTOMATA_H
#define _NONDETERMINISTIC_FINITE_AUTOMATA_H

#include "PCH.h"
#include "FiniteAutomata.h"
#include "DeterministicFiniteAutomata.h"

class NondeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		NondeterministicFiniteAutomata() : FiniteAutomata() { }
		NondeterministicFiniteAutomata(std::ifstream& ifs);
		NondeterministicFiniteAutomata(NondeterministicFiniteAutomata const& source) : FiniteAutomata(source) { }
		NondeterministicFiniteAutomata(uint32_t const& states, uint32_t const& initialState, 
			Vector<uint32_t> const& finalStates, TransitionMap const& transitionFunction) : 
			FiniteAutomata(states, initialState, finalStates, transitionFunction) { }

		void Reverse() override;

		bool IsAccepted(String const& word) const override;

		String GenerateWord(uint32_t const& length) const override;
		String GetRegularExpression() const override;

		DFA ToDFA() const;

	private:
		StatesSet LambdaClosure(uint32_t const& state) const;
		StatesSet LambdaClosure(StatesSet const& states) const;

		StatesSet MoveTo(uint32_t const& state, char const& key) const;
		StatesSet MoveTo(StatesSet const& states, char const& key) const;

		Set<char> GetAlphabet() const;
};

typedef NondeterministicFiniteAutomata NFA;

#endif

