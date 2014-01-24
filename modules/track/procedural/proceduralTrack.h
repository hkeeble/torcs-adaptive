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

#include "procedural\pSeg.h"
#include "procedural\pTrackState.h"
#include "procedural\pDefs.h"
#include "procedural\pTrack.h"

#include <string>

// PROCEDURAL SPECIFIC FUNCTIONS AND DEFINES
namespace procedural
{
	/* Instance of the current track */
	extern pTrack* proceduralTrack;

	#define TrackState		proceduralTrack->state

	// TA INTERFACE FUNCTIONS
	tTrack*				PInitTrack			(int trkLength);
	void				PShutDown			();
	void				PAddSegment			(pSeg seg, pTrack* trInfo);
	pTrack*				PGetTrackInfo		();
}

#endif // _TORCS_ADAPTIVE_H_