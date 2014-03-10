/*
	File: proceduralTrack.h
	Desc: Header file for procedural tracks. Contains extern declaration of a procedural track object, and functions used as interface into this module when managing
		  a procedural track.
	Author: Henri Keeble
*/
#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#include "raceman.h"
#include "portability.h"
#include "track.h"

#include "procedural\PSeg.h"
#include "procedural\PTrackState.h"
#include "procedural\pDefs.h"
#include "procedural\PTrack.h"

#include <string>

// PROCEDURAL SPECIFIC FUNCTIONS AND DEFINES
namespace procedural
{
	#define INITIAL_SEG_GEN 5

	/* Builds an initial track, loading its information from the given XML file. */
	tTrack*	BuildTrack(const char *const fName);

	// TA INTERFACE FUNCTIONS

	/* Initializes the procedural track's track structure and AC file. */
	void PInitTrack(PTrack* track);

	/* Shuts down the procedural track. */
	void PShutDown(PTrack* track);

	/* Adds the given segment to the given procedural track. */
	void PAddSegment(PSeg seg, PTrack* trInfo);

	/* Generates a track description consisting of a single segment, using the given configuration path. Used for smoother procedural generation. */
	tTrack* PGenerateSingleSegment(std::string configPath, tTrackSeg* seg);
}

#endif // _TORCS_ADAPTIVE_H_