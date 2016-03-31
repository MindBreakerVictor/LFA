#include "FiniteAutomata.h"

bool FiniteAutomata::isFinalState(const uint& state) const
{
	std::vector<uint>::const_iterator i;
	for (i = _finalStates.begin(); i != _finalStates.end(); i++)
		if ((*i) == state)
			return true;

	return false;
}
