/*
	File: Util.cpp
	Desc: Definitions for utility functions.
	Author: Henri Keeble
	Created: 03/10/2013
	Last Edit: 03/10/2013
*/

#include "Util.h"

namespace torcsAdaptive
{
	std::string dbleToStr(double dble)
	{
		std::stringstream s;
		s << dble;
		return s.str();
	}

	char* conChar(const char* a, const char* b)
	{
		char* result;
		result = (char*)calloc(strlen(a)+strlen(b)+1, sizeof(char));
		strcpy(result, a);
		strcat(result, b);
		
		return result;
	}
}