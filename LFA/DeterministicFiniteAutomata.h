#ifndef DETERMINISTIC_FINITE_AUTOMATA_H
#define DETERMINISTIC_FINITE_AUTOMATA_H

#include "FiniteAutomata.h"

class DeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		DeterministicFiniteAutomata(std::ifstream& ifs);
		DeterministicFiniteAutomata(const DeterministicFiniteAutomata& source) : 
			FiniteAutomata(source._states, source._initialState, source._finalStates), 
			_transitionTabel(source._transitionTabel) { }

		bool isAccepted(const String& word) const;
		std::vector<bool> getReachableStates() const;
		String generateWord(const uint& length) const;

	private:
		std::vector<std::map<char, uint>> _transitionTabel;	// δ - transition function

		void generateWord(const uint& state, const uint& length, String& word) const;
};

typedef DeterministicFiniteAutomata DFA;

#endif
