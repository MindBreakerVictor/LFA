#include "PCH.h"
#include "RegularExpression.h"

bool RegularExpression::IsInParentheses(String const& regularExpression)
{
	return ((*regularExpression.begin() == '(') && (*regularExpression.rbegin() == ')'));
}

String& RegularExpression::Parenthesize(String* regex)
{
	regex->insert(regex->begin(), '(');
	regex->insert(regex->end(), ')');
	
	return *regex;
}

String& RegularExpression::Star(String* regex)
{
	regex->insert(regex->end(), '*');

	return *regex;
}

String RegularExpression::Parenthesize(String const& regex)
{
	String res = regex;
	res.insert(res.begin(), '(');
	res.insert(res.end(), ')');

	return res;
}

String RegularExpression::Star(String const& regex)
{
	String res = regex;
	res.insert(res.end(), '*');

	return res;
}

