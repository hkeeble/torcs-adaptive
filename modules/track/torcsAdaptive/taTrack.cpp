/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
*/

#include "torcsAdaptive.h"
#include "..\trackinc.h"

namespace torcsAdaptive
{
	// static pointer to the adaptive track, scoped to single file
	static tTrack* taTrack;
	
	// Contains all current adaptive track information
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

	/* Initialize torcs-adaptive track */
	tTrack* TaInitTrack(int trkLength)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		GfOut("Setting Track 3D Description Loader Options...\n");
		ssgLoaderOptions* lopts = new ssgLoaderOptions();
		lopts->setModelDir("tracks\\adaptive\\taTrack1");
		lopts->setTextureDir("data\\textures");

		GfOut("Initializing track info object...\n");
		if(trInfo)
			delete trInfo;
		trInfo = new taTrackInfo("taTrack1.ac", "tracks/adaptive/taTrack1/", lopts); 
		if(!trInfo)
			GfFatal("Error initializing track info object!\n");

		GfOut("Allocating initial memory for track...\n");
		if(taTrack)
			delete taTrack;
		taTrack	=  new tTrack(); 
		if(!taTrack)
			GfFatal("Error allocating memory for adaptive track!\n");

		// Main info
		GfOut("Assigning Main Track Info...\n");
		char* fName = new char[strlen(GetLocalDir()) + strlen("tracks/adaptive/taTrack1/taTrack1.xml")];
		strcpy(fName, GetLocalDir());
		strcat(fName, "tracks/adaptive/taTrack1/taTrack1.xml");
		taTrack = TrackBuildv1(fName); // Load in track details, should return with no segments;

		// Initialize Sides
		InitSides(taTrack->params, taTrack);

		// Add Initial Segment
		GfOut("Adding Initial Segment...\n");
		TaAddSegment(taSeg(TR_NORMAL, TR_STR, TrackState.curSegIndex, TR_MAIN, 0, 200.f), taTrack);

		// Generate Initial 3D Description
		GenerateTrack(taTrack, taTrack->params, (char*)trInfo->GetACPathAndName(), NULL, NULL, NULL, 0);

		return taTrack;
	}

	taTrackInfo* TaGetTrackInfo()
	{
		return trInfo;
	}

	/* Release all torcs-adaptive track module resources */
	void TaShutDown()
	{
		TrackShutdown();

		if(trInfo)
			delete trInfo;
	}
}