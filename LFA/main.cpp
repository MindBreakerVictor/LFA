#include "PCH.h"
#include "DeterministicFiniteAutomata.h"
#include "NedeterministicFiniteAutomata.h"

void checkWord(const FiniteAutomata& fa)
{
	std::string s;
	std::getline(std::cin, s);

	if (fa.isAccepted(s))
		std::cout << "Accepted\n";
	else
		std::cout << "Unaccepted\n";
}

void printReachableStates(const FiniteAutomata& fa)
{
	std::vector<bool> reachableStates(fa.getReachableStates());

	for (uint i = 0; i < reachableStates.size(); i++)
		if (reachableStates[i])
			std::cout << i << " ";

	std::cout << "\n";
}

void generateWord(const FiniteAutomata& fa)
{
	std::string s;
	uint lenght;

	std::cin >> lenght;
	s = fa.generateWord(lenght);

	if (!s.empty())
		std::cout << s;
	else
		std::cout << "There is no word of lenght " << lenght << " that can be accepted by the DFA.";
}

int main()
{
	std::ifstream ifs("dfa.in");
	const DFA dfa(ifs);

	// Check if a word is accepted by the dfa
	checkWord(dfa);
	
	// Prints all reachable states.
	printReachableStates(dfa);

	// Generate word
	generateWord(dfa);

	_getch();
	return 0;
}
