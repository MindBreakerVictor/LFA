#ifndef FINITE_AUTOMATA_H
#define FINITE_AUTOMATA_H

#include "PCH.h"

class FiniteAutomata
{
	public:
		virtual bool isAccepted(const String& word) const = 0;
		virtual std::vector<bool> getReachableStates() const = 0;
		virtual String generateWord(const uint& length) const = 0;

	protected:
		uint _states;						// Q - finite set of states, indexing starting at 0.
		uint _initialState;					// q0 - initial/start state.
		std::vector<uint> _finalStates;		// F - finite set of final states.

		FiniteAutomata() : _states(0), _initialState(0), _finalStates(0) { }
		FiniteAutomata(const uint& states, const uint& initialState, const std::vector<uint>& finalStates) : 
			_states(states), _initialState(initialState), _finalStates(finalStates) { }

		bool isFinalState(const uint& state) const;
};

#endif
