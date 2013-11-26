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
	/* Represents the lifetime of an adaptive track */
	class taTrackInfo
	{
	public:
		taTrackInfo();
		~taTrackInfo();

		taTrackState state;
		EntityDesc* trackDesc;
		tTrackSeg* root;
		char* acName;
		char* acPath;
	};

	/* Instance of the current track */
	extern taTrackInfo* trInfo;

	#define TrackState		trInfo->state
	#define TrackDesc		trInfo->trackDesc
	#define Root			trInfo->root
	#define ACName			trInfo->acName
	#define ACPath			trInfo->acPath
	#define LoaderOptions	trInfo->loaderOptions

	// TA INTERFACE FUNCTIONS
	tTrack*				TaInitTrack			(int trkLength);
	void				TaShutDown			();
	void				TaAddSegment		(taSeg seg, tTrack* taTrack);
	taTrackState		TaGetTrackState		();
	EntityDesc*			TaGetTrackDesc		();
	void				TaSetTrackDesc		(torcsAdaptive::EntityDesc*);
	char*				TaGetACName			();
	char*				TaGetACPath			();
	char*				TaGetAcPathAndName	();
	ssgLoaderOptions*	TaGetLoaderOptions	();
}

#endif // _TORCS_ADAPTIVE_H_