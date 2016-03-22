#include "PCH.h"
#include "DFA.h"

int main()
{
	std::ifstream ifs("dfa.in");
	const DFA dfa(ifs);

	/*
	std::string s;
	std::getline(std::cin, s);
	*/
	
	std::vector<bool> reachableStates(dfa.getReachableStates());
	for (uint i = 0; i < reachableStates.size(); i++)
		if (reachableStates[i])
			std::cout << i << " ";

	_getch();
	return 0;
}
