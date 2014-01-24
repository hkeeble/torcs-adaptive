/***************************************************************************

    file                 : trackinc.h
    created              : Sun Jan 30 22:57:40 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: trackinc.h,v 1.4 2002/05/19 12:16:37 torcs Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef _TRACKINC_H__
#define _TRACKINC_H__

#include "track.h"

extern void TrackShutdown(void);
extern void ReadTrack3(tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);
extern void ReadTrack4(tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);

extern tTrack *TrackBuildv1(char *trackfile);
extern tTrack *TrackBuildEx(char *trackfile);
extern tdble TrackHeightG(tTrackSeg *seg, tdble x, tdble y);
extern tdble TrackHeightL(tTrkLocPos *p);
extern void TrackGlobal2Local(tTrackSeg *segment, tdble X, tdble Y, tTrkLocPos *p, int sides);
extern void TrackLocal2Global(tTrkLocPos *p, tdble *X, tdble *Y);
extern void TrackSideNormal(tTrackSeg*, tdble, tdble, int, t3Dd*);
extern void TrackSurfaceNormal(tTrkLocPos *p, t3Dd *norm);
extern tRoadCam *TrackGetCamList(void);
extern tdble TrackSpline(tdble p0, tdble p1, tdble t0, tdble t1, tdble t);

// TORCS-ADAPTIVE EXTERN ADDITIONS
// General Initialization
/* Defined as extern here for use outside of track3.cpp, for use in pTrack.cpp (static declarations also removed in track3.cpp) */
extern void				GetTrackHeader	(void *TrackHandle);
extern inline void		SetTrack		(tTrack* track, char* trFile);
extern void				ReadTrack3      (tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);
extern void             InitSides       (void *TrackHandle, tTrack *theTrack);
extern tTrackSurface*   AddTrackSurface (void *TrackHandle, tTrack *theTrack, const char *material);
extern void             AddSides        (tTrackSeg *curSeg, void *TrackHandle, tTrack *theTrack, int curStep, int steps);

#endif /* _TRACKINC_H__ */ 
