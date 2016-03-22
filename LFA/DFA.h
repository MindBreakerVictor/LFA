#ifndef DFA_H
#define DFA_H

#include "PCH.h"

// Homework I: Given an unsigned integer n generate a word accepted by the dfa. Using bfs & tree.
// Homework II: Given a NFA or λ-NFA, transform it to a DFA.
class DFA
{
	public:
		DFA(std::ifstream& input);
		DFA(const DFA& source) : states(source.states), startState(source.startState), finalStates(source.finalStates), transitionTabel(source.transitionTabel) { }

		bool checkString(const std::string& string) const;
		std::vector<bool> getReachableStates() const;
		std::string generateWord(const uint& lenght) const;

	private:
		uint states;	// Q - finite nonempty set of states, indexing starting at 0.
		uint startState;	// s - initial/start state
		std::vector<uint> finalStates;	// F - final state set
		std::vector<std::map<char, uint>> transitionTabel;	// δ - transition function

		bool isFinal(uint state) const;
};

#endif
