#ifndef _TA_DEFS_
#define _TA_DEFS_

#include "../../../windows/include/plib/ssg.h"

namespace torcsAdaptive
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
	
	// Vector Struct
	struct Vec3
	{
		Vec3();
		tdble x;
		tdble y;
		tdble z;
	};
}
#endif _TA_DEFS_