#ifndef LFA_LIB_FINITE_AUTOMATA_H
#define LFA_LIB_FINITE_AUTOMATA_H

#include "PCH.h"

typedef Set<uint32> StatesSet;
typedef Vector<uint32> StatesVector;
typedef Pair<uint32, char> TransitionPair;
typedef Map<TransitionPair, StatesVector> TransitionMap;
typedef StatesVector::const_iterator StatesConstIterator;
typedef StatesSet::const_iterator StatesSetConstIterator;
typedef TransitionMap::const_iterator TransitionMapConstIterator;

class NondeterministicFiniteAutomata;

class FiniteAutomata
{
	public:
		virtual void Reverse() = 0;
		void RemoveState(uint32 const& state);
		void RemoveUnreachableStates();

		bool HasStates() const { return (_states != 0) ? true : false; }
		bool HasFinalStates() const { return !_finalStates.empty(); }
		bool HasTransitions() const { return !_transitionFunction.empty(); }

		virtual bool IsAccepted(String const& word) const = 0;

		virtual String GenerateWord(uint32 const& length) const = 0;

		virtual void Minimize();

		StatesSet GetInconclusiveStates() const;
		StatesSet GetFinalStates() const;

		Set<char> GetAlphabet() const;

		Vector<bool> GetReachableStates() const;

		NondeterministicFiniteAutomata GetReverse() const;

		FiniteAutomata& operator=(FiniteAutomata const& source);

	protected:
		uint32 _states;
		uint32 _initialState;
		StatesVector _finalStates;
		TransitionMap _transitionFunction;

		FiniteAutomata() { }
		FiniteAutomata(FiniteAutomata const& source) : _states(source._states), _initialState(source._initialState),
			_finalStates(source._finalStates), _transitionFunction(source._transitionFunction) { }
		FiniteAutomata(uint32 const& states, uint32 const& initialState, 
			StatesVector const& finalStates, TransitionMap const& transitionFunction) : 
			_states(states), _initialState(initialState), _finalStates(finalStates), 
			_transitionFunction(transitionFunction) { }

		bool IsFinalState(uint32 const& state) const;
		bool IsFinalState(StatesSet const& state) const;
};

typedef FiniteAutomata FA;

#endif

