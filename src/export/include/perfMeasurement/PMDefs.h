/**
 *	@file PMDefs.h
 *	@author Henri Keeble
 *	@brief Definitions for some commonly used macros in the PM library.
*/

#ifndef _PM_DEFS_H_
#define _PM_DEFS_H_

#include "tgf.h"
#include <stdio.h>

namespace perfMeasurement
{
#define PERFMEASURE_UPDATE_INTERVAL 0.1

#define NULL_SKILL_LEVEL -1.f

#ifdef _DEBUG
#define  pmOut(...) printf("perfMeasurement >> ", __VA_ARGS__); \
					std::cout << std::endl;
	#else
		#define pmOut(out)
	#endif // _DEBUG

	/*! Represents a 2D Point. */
	struct PMPoint2D
	{
		PMPoint2D() { };
		PMPoint2D(tdble x, tdble y) { this->x = x; this->y = y; }

		tdble x;
		tdble y;
	};
}

#endif // _PM_DEFS_H_