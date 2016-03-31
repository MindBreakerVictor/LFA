#ifndef NEDETERMINISTIC_FINITE_AUTOMATA_H
#define NEDETERMINISTIC_FINITE_AUTOMATA_H

#include "DeterministicFiniteAutomata.h"

class NedeterministicFiniteAutomata : public FiniteAutomata
{
	public:
		NedeterministicFiniteAutomata(std::ifstream& ifs);
		NedeterministicFiniteAutomata(const NedeterministicFiniteAutomata& source) : 
			FiniteAutomata(source._states, source._initialState, source._finalStates), 
			_transitionTabel(source._transitionTabel) { }

		bool isAccepted(const String& word) const { return isAccepted(_initialState, word); }
		std::vector<bool> getReachableStates() const;
		String generateWord(const uint& length) const;

	private:
		std::vector<std::multimap<char, uint>> _transitionTabel;	// δ - transition function

		bool isAccepted(const uint& state, const String& word) const;

		void generateWord(const uint& state, const uint& length, String& word) const;

		std::vector<uint> lambdaClose(uint state) const;
		std::vector<uint> lambdaClose(std::vector<uint> stateSet) const;
		std::vector<uint> move(uint state, const char& key) const;
		std::vector<uint> move(std::vector<uint> stateSet, const char& key) const;
};

typedef NedeterministicFiniteAutomata NFA;

#endif
