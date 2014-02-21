/*
	File: pDefs.h
	Author: Henri Keeble
	Desc: Some preprocessor defines and other definitions common to many procedural translation units
*/

#ifndef _P_DEFS_
#define _P_DEFS_

#include "../../../windows/include/plib/ssg.h"

namespace procedural
{
	typedef ssgEntity EntityDesc;

	// Maximum and minimum ranges
	#define MIN_LENGTH 100
	#define MAX_LENGTH 200

	#define MAX_ARC 1.5f
	#define MIN_ARC 0.5f

	#define MAX_RADIUS 90.f
	#define MIN_RADIUS 25.f
	
#ifdef _DEBUG
	#define pOut(out) std::cout << "procedural >>" + std::string(out)
#else
	#define pOut(out)
#endif // _DEBUG

	enum PSegType
	{
		Straight = 0,
		Corner = 1
	};
}
#endif _P_DEFS_