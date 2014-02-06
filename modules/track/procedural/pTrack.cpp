/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
*/

#include "proceduralTrack.h"
#include "..\trackinc.h"

namespace procedural
{
	/* Initialize torcs-adaptive track */
	tTrack* PInitTrack(PTrack* pTrack, int trkLength, bool raceOnConsole)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		GfOut("Initializing track info object...\n");

		// Main info
		GfOut("Assigning Main Track Info...\n");
		PGenerateInitialTrack(pTrack->trackCache, pTrack->GetXMLPathAndName());

		// Initialize Sides
		InitSides(pTrack->trackCache->params, pTrack->trackCache);

		// Add Initial Segments
		GfOut("Adding Initial Segment...\n");
		PAddSegment(PSegFactory::GetInstance()->CreateSegStr(0, 500.f), pTrack);
		PAddSegment(PSegFactory::GetInstance()->CreateSegCnr(1, PCornerType::CTLeft, 90.f, 0.f, 0.f, 1.5f), pTrack);
		PAddSegment(PSegFactory::GetInstance()->CreateSegStr(2, 500.f), pTrack);

		// Generate Initial 3D Description
		if (!raceOnConsole)
			GenerateTrack(pTrack->trackCache, pTrack->trackCache->params, (char*)pTrack->GetACPathAndName(), nullptr, 0, 0, 0);

		return pTrack->trackCache;
	}

	void PGenerateInitialTrack(tTrack* track, const char *const fName)
	{
		char* fileName = new char[strlen(GetLocalDir()) + strlen(fName)];
		strcpy(fileName, GetLocalDir());
		strcat(fileName, fName);
		track = TrackBuildv1(const_cast<char*>(fName)); // Load in track details, should return with no segments;
	}

	void PGenerateNewSegment(PTrack* track)
	{
		// Create single segment track using last segment on track given
		tTrack* trk = new tTrack();
		trk->seg = new tTrackSeg(*track->GetEnd());

		// Generate initial track information
		PGenerateInitialTrack(trk, track->GetXMLPathAndName());
		
		// Initialize Sides
		InitSides(trk->params, trk);

		// Generate 3D description in temporary file
		GenerateTrack(trk, trk->params, (char*)track->GetTempACPathAndName(), nullptr, 0, 0, 0);
	}

	/* Release all torcs-adaptive track module resources */
	void PShutDown(PTrack* track)
	{
		TrackShutdown();

		if (track)
			delete track;
	}
}