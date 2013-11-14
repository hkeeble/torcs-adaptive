/*
	File: torcsAdaptive.h
	Desc: header file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 06/11/2013
*/
#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#include "raceman.h"
#include "portability.h"
#include "track.h"

#include "trackgen.h"

#include "torcsAdaptive/taSeg.h"
#include "torcsAdaptive/taTrackState.h"
#include "torcsAdaptive/taDefs.h"

#include <string>

// TORCS-ADAPTIVE SPECIFIC FUNCTIONS AND DEFINES
namespace torcsAdaptive
{
	// TA INTERFACE FUNCTIONS
	tTrack*			TaInitTrack			(int trkLength);
	void			TaShutDown			();
	void			TaAddSegment		(taSeg seg, tTrack* taTrack);
	void			TaAddSegment2		(taSeg seg, tTrack* taTrack);
	taTrackState*	TaGetTrackState		();
	EntityDesc*		TaGetTrackDesc		();
	char*			TaGetACName			();

	// Internal Use Only
	void TaInitTrackState ();

	// Instance of TrackState
	extern taTrackState* trackState;

	// Current 3D Description
	extern EntityDesc*	trackDesc;

	// Current Root
	extern tTrackSeg* root;

	// Current AC Name
	extern char* acName;
}

#endif // _TORCS_ADAPTIVE_H_