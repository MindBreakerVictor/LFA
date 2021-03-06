#ifndef LFA_LIB_REGULAR_EXPRESSION_H
#define LFA_LIB_REGULAR_EXPRESSION_H

#include "PCH.h"

namespace RegularExpression
{
	bool IsInParentheses(String const& regularExpression);

	String& Parenthesize(String* regex);
	String Parenthesize(String const& regex);

	String& Star(String* regex);
	String Star(String const& regex);
}

#endif

