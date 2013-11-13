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
/* Defined as extern here for use outside of track3.cpp, for use in taTrack.cpp (static declarations also removed in track3.cpp) */
extern void				GetTrackHeader	(void *TrackHandle);
extern inline void		SetTrack		(tTrack* track, char* trFile);
extern void				ReadTrack3      (tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);
extern void             InitSides       (void *TrackHandle, tTrack *theTrack);
extern tTrackSurface*   AddTrackSurface (void *TrackHandle, tTrack *theTrack, const char *material);
extern void             AddSides        (tTrackSeg *curSeg, void *TrackHandle, tTrack *theTrack, int curStep, int steps);

// TORCS-ADAPTIVE SPECIFIC FUNCTIONS AND DEFINES
namespace torcsAdaptive
{
	// Vector Struct
	struct Vec3
	{
		Vec3();
		tdble x;
		tdble y;
		tdble z;
	};

	#define TA_LENGTH_PER_SEG	40 // Length in meters of each segment

	// Surface Array Indices
	#define TA_SF_INDEX_ROAD	0
	#define TA_SF_INDEX_BARRIER 1

	// Defaults
	#define DEFAULT_SEG_STYLE	TR_WALL
	#define DEFAULT_SEG_TYPE2	TR_MAIN
	#define STEPS 1 // Steps Per Segment

	// TA FUNCTIONS
	extern tTrack*	TaInitTrack			(int trkLength);
	extern void		TaInitTrackState	();
	extern void		TaShutDown			();
	extern void		TaAddSegment		(taSeg seg, tTrack* taTrack);
	extern void		TaAddSegment2		(taSeg seg, tTrack* taTrack);

	/* Used to store the current state of the track */
	struct TrackState
	{
		TrackState();
		tTrackSeg* root;
		int	curSegIndex, envIndex;
		tdble totLength, wi2, grade, xr, yr, xl, yl, radius;
	};

	/* Instance of TrackState */
	extern TrackState* trackState;
}

#endif // _TORCS_ADAPTIVE_