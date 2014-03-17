/*
	File: PTrackBuild.h
	Author: Henri Keeble
	Desc: Track building functions from the track library used by the procedural library. All extern functions are defined in track.cpp, or track3.cpp in the track library. The procedural
		  library is therefore dependent upon the track library. This is intentional - procedural should be dependent upon track, and not vice versa. TORCS Adaptive libraries will not build
		  without TORCS' core modules and libraries.
*/

#ifndef _TRACK_BUILD_H_
#define _TRACK_BUILD_H_

#include "track.h"

extern void				GetTrackHeader	(void *TrackHandle, tTrack* track);
extern void				ReadTrack3      (tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);
extern void				ReadTrack4		(tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);
extern void				TrackShutdown	(void);
extern void             InitSides       (void *TrackHandle, tTrack *theTrack);
extern tTrackSurface*   AddTrackSurface (void *TrackHandle, tTrack *theTrack, const char *material);
extern void             AddSides        (tTrackSeg *curSeg, void *TrackHandle, tTrack *theTrack, int curStep, int steps);
extern tdble			TrackSpline(tdble p0, tdble p1, tdble t0, tdble t1, tdble t);

#endif // _TRACK_BUILD_H_