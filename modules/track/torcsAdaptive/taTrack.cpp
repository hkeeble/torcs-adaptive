/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 07/11/2013
*/

#include "torcsAdaptive.h"
#include "..\trackinc.h"

namespace torcsAdaptive
{
	static tTrack*		taTrack;
	
	taTrackState*	trackState;
	EntityDesc*		trackDesc;
	tTrackSeg*		root;
	char*			acName;

	// Track accessors for ease of reading
	#define taGraphicInfo	taTrack->graphic
	#define taSegments		taTrack->seg
	#define taSurfaces		taTrack->surfaces
	#define taPits			taTrack->pits

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

		// Main info
		GfOut("Assigning Main Track Info...\n");
		char* fName = new char[strlen(GetLocalDir()) + strlen("tracks/adaptive/taTrack1/taTrack1.xml")];
		strcpy(fName, GetLocalDir());
		strcat(fName, "tracks/adaptive/taTrack1/taTrack1.xml");
		taTrack = TrackBuildv1(fName); // Load in track details, should return with no segments;

		// Get AC File Name
		acName = (char*)GfParmGetStr(taTrack->params, TRK_SECT_GRAPH, TRK_ATT_3DDESC, "track.ac");

		// Initialize Track State
		TaInitTrackState();

		// Initialize 3D Description
		trackDesc = NULL;

		// Initialize Root
		root = NULL;

		// Add Initial Segment
		GfOut("Adding Initial Segments...\n");
		TaAddSegment(taSeg(TR_NORMAL, TR_STR, trackState->curSegIndex, TR_MAIN, 0), taTrack);

		// Generate Initial 3D Description
		GenerateTrack(taTrack, taTrack->params, acName, NULL, NULL, NULL);

		// Track Min and Max (Make Assumptions Here?)
		taTrack->min.x = 0;
		taTrack->min.y = 0;
		taTrack->min.z = 0;
		taTrack->max.x = 1000;
		taTrack->max.y = 1000;
		taTrack->max.z = 1000;

		return taTrack;
	}

	 void TaInitTrackState()
	 {
		 if(trackState)
			delete trackState;
		trackState	=  new taTrackState(); // Allocate memory for track state

		if(!trackState)
			GfFatal("Error allocating memory for track state!\n");

		InitSides(taTrack->params, taTrack);
	 }

	 /* Retrieve Current Track State */
	 taTrackState* TaGetTrackState()
	 {
		 return trackState;
	 }

	 /* Retrieve Current Track 3D Description */
	 ssgEntity*	TaGetTrackDesc()
	 {
		 return trackDesc;
	 }

	 char* TaGetACName()
	 {
		 return acName;
	 }

	void TaShutDown()
	{
		TrackShutdown();

		if(trackState)
			delete trackState;
		if(trackDesc)
			delete trackDesc;
		if(acName)
			delete[] acName;
	}
}