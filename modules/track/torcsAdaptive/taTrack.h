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

// TORCS-ADAPTIVE EXTERN ADDITIONS
// General Initialization
extern void				GetTrackHeader	(void *TrackHandle);
extern inline void		SetTrack		(tTrack* track, char* trFile);
extern void				ReadTrack3(tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);

// TORCS-ADAPTIVE SPECIFIC FUNCTIONS AND DEFINES
namespace torcsAdaptive
{
	#define TA_LENGTH_PER_SEG	40 // Length in meters of each segment

	// Surface Array Indices
	#define TA_SF_INDEX_ROAD	0
	#define TA_SF_INDEX_BARRIER 1

	// TA FUNCTIONS
	extern tTrack*	TaInitTrack			(int trkLength);
	extern void		TaShutDown			();
	extern void		TaAddSegment		(taSeg seg);
}

#endif // _TORCS_ADAPTIVE_