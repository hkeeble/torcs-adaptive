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
}
#endif _P_DEFS_