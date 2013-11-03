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
}