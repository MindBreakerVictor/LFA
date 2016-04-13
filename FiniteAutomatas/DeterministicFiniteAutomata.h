#pragma once

#include "PCH.h"
#include "FiniteAutomata.h"

class DeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		DeterministicFiniteAutomata() : FiniteAutomata() { }
		DeterministicFiniteAutomata(std::ifstream& ifs);
		DeterministicFiniteAutomata(const DeterministicFiniteAutomata& source) : FiniteAutomata(source) { }
		DeterministicFiniteAutomata(const uint32_t& states, const uint32_t& initialState, const Vector<uint32_t>& finalStates, 
			const TransitionMap& transitionFunction) : FiniteAutomata(states, initialState, finalStates, transitionFunction) { }

		bool isAccepted(const String& word) const override;

		String generateWord(const uint32_t& length) const override;

	private:
		bool generateWord(uint32_t currentState, uint32_t length, String& word) const;
};

typedef DeterministicFiniteAutomata DFA;

