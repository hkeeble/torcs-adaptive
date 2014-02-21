/*
	File: Util.h
	Desc: Declarations for utility functions.
	Author: Henri Keeble
	Created: 03/10/2013
	Last Edit: 06/10/2013
*/
#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <sstream>

namespace torcsAdaptive
{
	// Convert Double to String
	extern std::string dbleToStr(double dble);
	
	// Concatenate Two Const Char*
	extern char* conChar(const char* a, const char* b);
}

#endif // _UTIL_H_