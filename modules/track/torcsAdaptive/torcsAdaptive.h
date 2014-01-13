/*
	File: torcsAdaptive.h
	Desc: header file for torcsAdaptive
	Author: Henri Keeble
*/
#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#include "raceman.h"
#include "portability.h"
#include "track.h"

#include "trackgen.h"

#include "torcsAdaptive\taSeg.h"
#include "torcsAdaptive\taTrackState.h"
#include "torcsAdaptive\taDefs.h"
#include "torcsAdaptive\taTrack.h"

#include <string>

// TORCS-ADAPTIVE SPECIFIC FUNCTIONS AND DEFINES
namespace torcsAdaptive
{
	/* Instance of the current track */
	extern taTrack* adaptiveTrack;

	#define TrackState		adaptiveTrack->state

	// TA INTERFACE FUNCTIONS
	tTrack*				TaInitTrack			(int trkLength);
	void				TaShutDown			();
	void				TaAddSegment		(taSeg seg, taTrack* trInfo);
	taTrack*			TaGetTrackInfo		();
}

#endif // _TORCS_ADAPTIVE_H_