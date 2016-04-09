#pragma once

#include "PCH.h"

class FiniteAutomata
{
	public:
		virtual bool isAccepted(const String& word) const = 0;

		virtual String generateWord(const uint32_t& length) const = 0;

		Vector<bool> getReachableStates() const;

	protected:
		uint32_t _states;
		uint32_t _initialState;
		Vector<uint32_t> _finalStates;
		TransitionMap _transitionFunction;

		FiniteAutomata() { }
		FiniteAutomata(const FiniteAutomata& source) : _states(source._states), _initialState(source._initialState), 
			_finalStates(source._finalStates), _transitionFunction(source._transitionFunction) { }
		FiniteAutomata(const uint32_t& states, const uint32_t& initialState, const Vector<uint32_t>& finalStates, 
			const TransitionMap& transitionFunction) : _states(states), _initialState(initialState), 
			_finalStates(finalStates), _transitionFunction(transitionFunction) { }

		bool isFinalState(const uint32_t& state) const;
		bool isFinalState(const Set<uint32_t>& state) const;
};

typedef FiniteAutomata FA;

