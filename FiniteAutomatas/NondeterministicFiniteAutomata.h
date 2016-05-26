#ifndef LFA_LIB_NONDETERMINISTIC_FINITE_AUTOMATA_H
#define LFA_LIB_NONDETERMINISTIC_FINITE_AUTOMATA_H

#include "PCH.h"
#include "FiniteAutomata.h"
#include "DeterministicFiniteAutomata.h"

class NondeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		NondeterministicFiniteAutomata() : FiniteAutomata() { }
		NondeterministicFiniteAutomata(std::ifstream& ifs);
		NondeterministicFiniteAutomata(NondeterministicFiniteAutomata const& source) : FiniteAutomata(source) { }
		NondeterministicFiniteAutomata(uint32 const& states, uint32 const& initialState, 
			Vector<uint32> const& finalStates, TransitionMap const& transitionFunction) : 
			FiniteAutomata(states, initialState, finalStates, transitionFunction) { }

		void Reverse() override;

		bool IsAccepted(String const& word) const override;

		String GenerateWord(uint32 const& length) const override;

		DFA ToDFA() const;

	private:
		StatesSet LambdaClosure(uint32 const& state) const;
		StatesSet LambdaClosure(StatesSet const& states) const;

		StatesSet MoveTo(uint32 const& state, char const& key) const;
		StatesSet MoveTo(StatesSet const& states, char const& key) const;
};

typedef NondeterministicFiniteAutomata NFA;

#endif

