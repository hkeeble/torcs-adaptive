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
	#define TA_LENGTH_PER_SEG	40 // Length in meters of each segment

	// Surface Array Indices
	#define TA_SF_INDEX_ROAD	4
	#define TA_SF_INDEX_BARRIER 0

	// Defaults
	#define DEFAULT_SEG_STYLE	TR_WALL
	#define DEFAULT_SEG_TYPE2	TR_MAIN
	#define STEPS 1 // Steps Per Segment

	typedef ssgEntity EntityDesc;

	// Maximum and minimum ranges
	#define MAX_LENGTH 200.f
	#define MIN_LENGTH 100.f

	#define MAX_ARC 2.5f
	#define MIN_ARC 0.5f

	#define MAX_RADIUS 400.f
	#define MIN_RADIUS 25.f
	
	#define taOut(out) printf("ta >> " out)

	enum PSegType
	{
		Straight = 0,
		Corner = 1
	};
}
#endif _P_DEFS_