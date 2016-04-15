Formal Languages & Automatons Project
Finite Automatons library.
Student: Stanescu Victor Laurentiu
Faculty of Mathematics & Computer Science, University of Bucharest

FiniteAutomata attributes:
_states represents the no. of states in the automaton. They are indexed starting from 0.
_initialState represents the index of initial(start) state of the automaton. Usually it is 0.
_finalStates is a vector of unsigned ints in which we keep every final state.
_transitionFunction is a map with key of type pair, of unsigned int and char, and value of type vector of unsigned ints.
DFAs have only one value in the vector, whilst NFAs have more values.
For a transition from state i to state j with symbol k in _transitionFunction we have (i, 'k')->{j}.

Input file:
numberOfStates initialState numberOfFinalStates
finalStates1 ..... finalStateN
state symbol transitonState
.
.
.