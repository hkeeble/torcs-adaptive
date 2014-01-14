/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
*/

#include "torcsAdaptive.h"
#include "..\trackinc.h"

namespace torcsAdaptive
{
	// Contains all current adaptive track information
	taTrack* adaptiveTrack;

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
		if(adaptiveTrack)
			delete adaptiveTrack;
		adaptiveTrack = new taTrack(new tTrack(), "taTrack1.ac", "tracks/adaptive/taTrack1/", lopts);
		if (!adaptiveTrack)
			GfFatal("Error initializing track info object!\n");

		// Main info
		GfOut("Assigning Main Track Info...\n");
		char* fName = new char[strlen(GetLocalDir()) + strlen("tracks/adaptive/taTrack1/taTrack1.xml")];
		strcpy(fName, GetLocalDir());
		strcat(fName, "tracks/adaptive/taTrack1/taTrack1.xml");
		adaptiveTrack->track = TrackBuildv1(fName); // Load in track details, should return with no segments;

		// Initialize Sides
		InitSides(adaptiveTrack->track->params, adaptiveTrack->track);

		// Add Initial Segment
		GfOut("Adding Initial Segment...\n");
		TaAddSegment(TaSegFactory::GetInstance()->CreateSegStr(0, 200.f), adaptiveTrack);
		TaAddSegment(TaSegFactory::GetInstance()->CreateSegStr(1, 200.f), adaptiveTrack);
		TaAddSegment(TaSegFactory::GetInstance()->CreateSegStr(2, 200.f), adaptiveTrack);

		// Generate Initial 3D Description
		GenerateTrack(adaptiveTrack->track, adaptiveTrack->track->params, (char*)adaptiveTrack->GetACPathAndName(), NULL, NULL, NULL, 0);

		return adaptiveTrack->track;
	}

	taTrack* TaGetTrackInfo()
	{
		return adaptiveTrack;
	}

	/* Release all torcs-adaptive track module resources */
	void TaShutDown()
	{
		TrackShutdown();

		if (adaptiveTrack)
			delete adaptiveTrack;
	}
}