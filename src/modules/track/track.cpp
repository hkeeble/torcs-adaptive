/***************************************************************************

    file                 : track.cpp
    created              : Sun Jan 30 22:54:56 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: track.cpp,v 1.13.2.4 2012/02/09 22:36:27 berniw Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <tgf.h>
#include <track.h>
#include <portability.h>
#include "procedural\PSeg.h"
#include "trackbuild.h"

const tdble DEGPRAD = 180.0 / PI;   /* degrees per radian */

static tTrack	*theTrack		= NULL;
static tRoadCam *theCamList		= NULL;
static void		*TrackHandle	= NULL;

/*
 * External function used to (re)build a track
 * from the track file
 */
tTrack *
TrackBuildv1(char *trackfile)
{
    TrackShutdown();

    theTrack = (tTrack*)calloc(1, sizeof(tTrack));
    theCamList = (tRoadCam*)NULL;

    theTrack->params = TrackHandle = GfParmReadFile (trackfile, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);
    
    theTrack->filename = _strdup(trackfile);

    GetTrackHeader(TrackHandle, theTrack);

    
    switch(theTrack->version) {
    case 0:
    case 1:
    case 2:
    case 3:
	ReadTrack3(theTrack, TrackHandle, &theCamList, 0);
	break;
    case 4:
	ReadTrack4(theTrack, TrackHandle, &theCamList, 0);
	break;
    }

    return theTrack;
}

/*
 Externally set the track pointer without calling TrackBuildv1 (used for torcs-adaptive track
 initialization)
*/
void SetTrack(tTrack* track, char* trFile)
{
	theTrack = track;
	theTrack->filename = _strdup(trFile);
}

tTrack *
TrackBuildEx(char *trackfile)
{
    void	*TrackHandle;

    theTrack = (tTrack*)calloc(1, sizeof(tTrack));
    theCamList = (tRoadCam*)NULL;

    theTrack->params = TrackHandle = GfParmReadFile (trackfile, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);
    
    theTrack->filename = _strdup(trackfile);

    GetTrackHeader(TrackHandle, theTrack);

    switch(theTrack->version) {
    case 0:
    case 1:
    case 2:
    case 3:
	ReadTrack3(theTrack, TrackHandle, &theCamList, 1);
	break;
    case 4:
	ReadTrack4(theTrack, TrackHandle, &theCamList, 1);
	break;
    }
    
    return theTrack;
}

static void
freeSeg(tTrackSeg *seg)
{
	if (seg->barrier[0]) {
		free(seg->barrier[0]);
	}
	if (seg->barrier[1]) {
		free(seg->barrier[1]);
	}
	if (seg->ext) {
		free(seg->ext->marks);
		free(seg->ext);
	}
	if (seg->lside) {
		freeSeg(seg->lside);
	}
	if (seg->rside) {
		freeSeg(seg->rside);
	}
	free(seg);
}

void
TrackShutdown(void)
{
	tTrackSeg *curSeg;
	tTrackSeg *nextSeg;
	tTrackSurface *curSurf;
	tTrackSurface *nextSurf;
	tRoadCam *curCam;
	tRoadCam *nextCam;

	if (!theTrack) {
		return;
	}

	nextSeg = theTrack->seg->next;
	do {
		curSeg = nextSeg;
		nextSeg = nextSeg->next;
		freeSeg(curSeg);
	} while (curSeg != theTrack->seg);

	curSurf = theTrack->surfaces;
	while (curSurf) {
		nextSurf = curSurf->next;
		free(curSurf);
		curSurf = nextSurf;
	}

	curCam = theCamList;
	if (curCam) {
	do {
		nextCam = curCam->next;
		free(curCam);
		curCam = nextCam;
	} while (curCam != theCamList);
	}
	theCamList = NULL;

	if (theTrack->pits.driversPits) free(theTrack->pits.driversPits);
	free(theTrack->graphic.env);
	free(theTrack->internalname);
	free(theTrack->filename);
	free(theTrack);

	GfParmReleaseHandle(TrackHandle);
	theTrack = NULL;
}
