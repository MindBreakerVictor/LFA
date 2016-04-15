#include "PCH.h"

Vector<Vector<String>> operator+(Vector<Vector<String>> const& vectorOne, Vector<Vector<String>> const& vectorTwo)
{
	assert(vectorOne.size() == vectorTwo.size());
	for (uint32_t i = 0; i < vectorOne.size(); i++)
		assert(vectorOne[i].size() == vectorTwo[i].size());

	Vector<Vector<String>> sum(vectorOne.size());

	for (uint32_t i = 0; i < vectorOne.size(); i++)
	{
		sum[i].resize(vectorOne[i].size(), "");

		for (uint32_t j = 0; j < vectorOne[i].size(); j++)
		{
			if (!vectorOne[i][j].empty() && !vectorTwo[i][j].empty())
				sum[i][j] = "(" + vectorOne[i][j] + "+" + vectorTwo[i][j] + ")";
			else if (vectorOne[i][j].empty() && !vectorTwo[i][j].empty())
				sum[i][j] = vectorTwo[i][j];
			else if (!vectorOne[i][j].empty() && vectorTwo[i][j].empty())
				sum[i][j] = vectorOne[i][j];
		}
	}

	return sum;
}

Vector<Vector<String>>& operator+=(Vector<Vector<String>>& vectorOne, Vector<Vector<String>> const& vectorTwo)
{
	vectorOne = vectorOne + vectorTwo;

	return vectorOne;
}

