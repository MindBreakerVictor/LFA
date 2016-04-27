#ifndef _PCH_H
#define _PCH_H

#include <assert.h>

#include <fstream>
#include <iostream>

#include <map>
#include <set>
#include <queue>
#include <stack>
#include <vector>
#include <string>

#include <iterator>
#include <algorithm>

using String = std::string;

template <class Type>
using Vector = std::vector<Type>;

template <class Type>
using Queue = std::queue<Type>;

template <class Type>
using Stack = std::stack<Type>;

template <class Type>
using Set = std::set<Type>;

template <class Type1, class Type2>
using Pair = std::pair<Type1, Type2>;

template <class KeyType, class ValueType>
using Map = std::map<KeyType, ValueType>;

#endif

