/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 07/11/2013
*/

#include "taTrack.h"
#include "..\trackinc.h"

namespace torcsAdaptive
{
	static tTrack*		taTrack;
	static int			segsAdded;
	
	TrackState*	trackState;

	// Track accessors for ease of reading
	#define taGraphicInfo taTrack->graphic
	#define taSegments	taTrack->seg
	#define taSurfaces taTrack->surfaces
	#define taPits taTrack->pits

	// Vector Constructor
	Vec3::Vec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	// ----- ta Functions ------
	tTrack* TaInitTrack(int trkLength)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		if(taTrack)
			delete taTrack;
		taTrack	=  new tTrack(); // Allocate memory for track

		if(!taTrack)
			GfFatal("Error allocating memory for adaptive track!\n");
		if(!trackState)
			GfFatal("Error allocating memory for track state!\n");

		// Main info
		GfOut("Assigning Main Track Info...\n");
		char* fName = new char[strlen(GetLocalDir()) + strlen("tracks/adaptive/taTrack1/taTrack1.xml")];
		strcpy(fName, GetLocalDir());
		strcat(fName, "tracks/adaptive/taTrack1/taTrack1.xml");
		taTrack = TrackBuildv1(fName); // Load in track details, should return with no segments;

		// Initialize Track State
		TaInitTrackState();

		// Add Initial Segments
		GfOut("Adding Initial Segments...\n");
		segsAdded = 0;
		for(int i = 0; i < 6; i++)
			TaAddSegment2(taSeg(TR_NORMAL, TR_STR, trackState->curSegIndex, TR_MAIN, 0), taTrack);

		// Track Min and Max (Make Assumptions Here?)
		taTrack->min.x = 0;
		taTrack->min.y = 0;
		taTrack->min.z = 0;
		taTrack->max.x = 1000;
		taTrack->max.y = 1000;
		taTrack->max.z = 1000;

		return taTrack;
	}

	/* TrackState Constructor */
	TrackState::TrackState()
	{
		envIndex = 0;
		curSegIndex = 0;
		totLength = 0;

		material = GfParmGetStr(taTrack->params, TRK_SECT_MAIN, TRK_ATT_SURF, TRK_VAL_ASPHALT);
		surface = AddTrackSurface(taTrack->params, taTrack, material);

		wi2 = taTrack->width/2.0f;
		grade = -100000.0;

		root = NULL;
		surface = NULL;
	}

	 void TaInitTrackState()
	 {
		 if(trackState)
			delete trackState;
		trackState	=  new TrackState(); // Allocate memory for track state

		InitSides(taTrack->params, taTrack);
	 }

	void TaShutDown()
	{
		TrackShutdown();

		if(trackState)
			delete trackState;
	}
}