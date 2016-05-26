#ifndef LFA_LIB_PCH_H
#define LFA_LIB_PCH_H

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

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef std::string String;

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

