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

#include "trackgen.h"

#include "procedural\PSeg.h"
#include "procedural\PTrackState.h"
#include "procedural\pDefs.h"
#include "procedural\PTrack.h"

#include <string>

// PROCEDURAL SPECIFIC FUNCTIONS AND DEFINES
namespace procedural
{
	/* Builds an initial track, loading its information from the given XML file. */
	tTrack*				BuildTrack(const char *const fName);

	// TA INTERFACE FUNCTIONS
	PTrack*				PInitTrack				(tdble trkLength, char* acname, char* xmlname, char* filepath, ssgLoaderOptions* lopts, bool raceOnConsole);
	void				PUpdateACFile			(PTrack* pTrack);
	void				PShutDown				(PTrack* track);
	void				PAddSegment				(PSeg seg, PTrack* trInfo);
}

#endif // _TORCS_ADAPTIVE_H_