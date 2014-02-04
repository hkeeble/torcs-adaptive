/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
*/

#include "proceduralTrack.h"
#include "..\trackinc.h"

namespace procedural
{
	// Contains all current adaptive track information
	PTrack* proceduralTrack;

	// Track accessors for ease of reading
	#define pGraphicInfo	PTrack->graphic
	#define pSegments		PTrack->seg
	#define pSurfaces		PTrack->surfaces
	#define pPits			PTrack->pits

	/* Initialize torcs-adaptive track */
	tTrack* PInitTrack(int trkLength, bool raceOnConsole)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		ssgLoaderOptions* lopts = nullptr;

		if (!raceOnConsole)
		{
			GfOut("Setting Track 3D Description Loader Options...\n");
			lopts = new ssgLoaderOptions();
			lopts->setModelDir("tracks\\adaptive\\taTrack1");
			lopts->setTextureDir("data\\textures");
		}

		GfOut("Initializing track info object...\n");
		if(proceduralTrack)
			delete proceduralTrack;
		proceduralTrack = new PTrack(new tTrack(), trkLength, "taTrack1.ac", "tracks/adaptive/taTrack1/", lopts);
		if (!proceduralTrack)
			GfFatal("Error initializing track info object!\n");

		// Main info
		GfOut("Assigning Main Track Info...\n");
		char* fName = new char[strlen(GetLocalDir()) + strlen("tracks/adaptive/taTrack1/taTrack1.xml")];
		strcpy(fName, GetLocalDir());
		strcat(fName, "tracks/adaptive/taTrack1/taTrack1.xml");
		proceduralTrack->trackCache = TrackBuildv1(fName); // Load in track details, should return with no segments;

		// Initialize Sides
		InitSides(proceduralTrack->trackCache->params, proceduralTrack->trackCache);

		// Add Initial Segment
		GfOut("Adding Initial Segment...\n");
		PAddSegment(PSegFactory::GetInstance()->CreateSegStr(0, 500.f), proceduralTrack);
		PAddSegment(PSegFactory::GetInstance()->CreateSegCnr(1, PCornerType::CTLeft, 90.f, 0.f, 0.f, 1.5f), proceduralTrack);
		PAddSegment(PSegFactory::GetInstance()->CreateSegStr(2, 500.f), proceduralTrack);

		// Generate Initial 3D Description
		if (!raceOnConsole)
			GenerateTrack(proceduralTrack->trackCache, proceduralTrack->trackCache->params, (char*)proceduralTrack->GetACPathAndName(), NULL, NULL, NULL, 0);

		return proceduralTrack->trackCache;
	}

	PTrack* PGetTrackInfo()
	{
		return proceduralTrack;
	}

	/* Release all torcs-adaptive track module resources */
	void PShutDown()
	{
		TrackShutdown();

		if (proceduralTrack)
			delete proceduralTrack;
	}
}