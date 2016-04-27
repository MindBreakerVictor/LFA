#ifndef _FINITE_AUTOMATA_H
#define _FINITE_AUTOMATA_H

#include "PCH.h"

using States = Vector<uint32_t>;
using StatesSet = Set<uint32_t>;
using TransitionPair = Pair<uint32_t, char>;
using TransitionMap =  Map<TransitionPair, States>;
using StatesConstIterator = States::const_iterator;
using StatesSetConstIterator = StatesSet::const_iterator;
using TransitionMapConstIterator = TransitionMap::const_iterator;

class FiniteAutomata
{
	public:
		bool HasStates() const { return (_states != 0) ? true : false; }
		bool HasFinalStates() const { return !_finalStates.empty(); }
		bool HasTransition() const { return !_transitionFunction.empty(); }
		virtual bool IsAccepted(String const& word) const = 0;

		virtual String GenerateWord(uint32_t const& length) const = 0;
		virtual String GetRegularExpression() const = 0;

		virtual Vector<bool> GetReachableStates() const final;

	protected:
		uint32_t _states;
		uint32_t _initialState;
		States _finalStates;
		TransitionMap _transitionFunction;

		FiniteAutomata() { }
		FiniteAutomata(FiniteAutomata const& source) : _states(source._states), _initialState(source._initialState),
			_finalStates(source._finalStates), _transitionFunction(source._transitionFunction) { }
		FiniteAutomata(uint32_t const& states, uint32_t const& initialState, 
			States const& finalStates, TransitionMap const& transitionFunction) : 
			_states(states), _initialState(initialState), _finalStates(finalStates), 
			_transitionFunction(transitionFunction) { }

		bool IsFinalState(uint32_t const& state) const;
		bool IsFinalState(StatesSet const& state) const;
};

typedef FiniteAutomata FA;

#endif

