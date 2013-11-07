/*
	File: torcsAdaptive.h
	Desc: header file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 06/11/2013
*/
#ifndef _TORCS_ADAPTIVE_
#define _TORCS_ADAPTIVE_

#include "raceman.h"
#include "portability.h"
#include <string>

namespace torcsAdaptive
{
	#define TA_LENGTH_PER_SEG	40 // Length in meters of each segment

	// Surface Array Indices
	#define TA_MAX_SF			2
	#define TA_SF_INDEX_ROAD	0
	#define TA_SF_INDEX_BARRIER 1

	// TA Functions
	extern tTrack*	TaInitTrack			(int trkLength);
	extern void		TaInitPits			();
	extern void		TaInitSurfaces		();
	extern void		TaInitGraphicInfo	();
	extern void		TaShutDown			();
	extern void		TaAddSegment		(taSeg seg);
}

#endif // _TORCS_ADAPTIVE_