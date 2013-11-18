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
	
	taTrackInfo* trInfo;

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

	// Track Info Definitions
	taTrackInfo::taTrackInfo()
	{
		trackDesc = NULL;
		root = NULL;
		acName = NULL;
		state = taTrackState();
	}

	taTrackInfo::~taTrackInfo()
	{
		if(trackDesc)
			delete trackDesc;
		if(root)
			delete root;
		if(acName)
			delete acName;
	}

	// ----- ta Functions ------
	tTrack* TaInitTrack(int trkLength)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		if(trInfo)
			delete trInfo;
		trInfo = new taTrackInfo(); // Allocate memory for track info

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
		ACName = "tracks/adaptive/taTrack1/taTrack1.ac";

		// Initialize Sides
		InitSides(taTrack->params, taTrack);

		// Initialize 3D Description
		TrackDesc = NULL;

		// Initialize Root
		Root = NULL;

		// Add Initial Segment
		GfOut("Adding Initial Segments...\n");
		TaAddSegment(taSeg(TR_NORMAL, TR_STR, TrackState.curSegIndex, TR_MAIN, 0, 50.f), taTrack);

		// Generate Initial 3D Description
		GenerateTrack(taTrack, taTrack->params, ACName, NULL, NULL, NULL, 0);

		return taTrack;
	}

	 /* Retrieve Current Track State */
	 taTrackState TaGetTrackState()
	 {
		 return TrackState;
	 }

	 /* Retrieve Current Track 3D Description */
	 ssgEntity*	TaGetTrackDesc()
	 {
		 return TrackDesc;
	 }

	 char* TaGetACName()
	 {
		 return ACName;
	 }

	void TaShutDown()
	{
		TrackShutdown();

		if(trInfo)
			delete trInfo;
	}
}