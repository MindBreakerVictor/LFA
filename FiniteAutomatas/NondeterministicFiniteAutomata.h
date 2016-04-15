#pragma once

#include "PCH.h"
#include "FiniteAutomata.h"
#include "DeterministicFiniteAutomata.h"

class NondeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		NondeterministicFiniteAutomata() : FiniteAutomata() { }
		NondeterministicFiniteAutomata(std::ifstream& ifs);
		NondeterministicFiniteAutomata(const NondeterministicFiniteAutomata& source) : FiniteAutomata(source) { }
		NondeterministicFiniteAutomata(const uint32_t& states, const uint32_t& initialState, const Vector<uint32_t>& finalStates,
			const TransitionMap& transitionFunction) : FiniteAutomata(states, initialState, finalStates, transitionFunction) { }

		bool isAccepted(const String& word) const override;

		String generateWord(const uint32_t& length) const override;
		String getRegularExpression() const override;

		DFA ToDFA() const;

	private:
		Set<uint32_t> lambdaClosure(const uint32_t& state) const;
		Set<uint32_t> lambdaClosure(const Set<uint32_t>& states) const;

		Set<uint32_t> moveTo(const uint32_t& state, const char& key) const;
		Set<uint32_t> moveTo(const Set<uint32_t>& states, const char& key) const;

		Set<char> getAlphabet() const;
};

typedef NondeterministicFiniteAutomata NFA;

