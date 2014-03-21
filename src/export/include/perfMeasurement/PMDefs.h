/*
	File: PMDefs.h
	Author: Henri Keeble
	Desc: Definitions for some commonly used macros in the PM library.
*/

#ifndef _PM_DEFS_H_
#define _PM_DEFS_H_

#include "tgf.h"

namespace perfMeasurement
{
	#define PERFMEASURE_UPDATE_INTERVAL 0.1

	#define NULL_SKILL_LEVEL -1.f

	#ifdef _DEBUG
		#define pmOut(out) std::cout << "perfMeasurement >>" + std::string(out)
	#else
		#define pmOut(out)
	#endif // _DEBUG

	struct PMPoint2D
	{
		PMPoint2D() { };

		tdble x;
		tdble y;
	};
}

#endif // _PM_DEFS_H_