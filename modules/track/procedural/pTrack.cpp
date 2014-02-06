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
	PTrack* PInitTrack(tdble trkLength, char* acname, char* xmlname, char* filepath, ssgLoaderOptions* lopts, bool raceOnConsole)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		GfOut("Initializing procedural track object...\n");
		PTrack* pTrack = new PTrack(new tTrack(), trkLength, acname, xmlname, filepath, lopts);

		// Main info
		GfOut("Assigning Main Track Info...\n");
		pTrack->trackCache = PGenerateInitialTrack(pTrack->GetXMLPathAndName());

		// Initialize Sides
		InitSides(pTrack->trackCache->params, pTrack->trackCache);

		// Add Initial Segments
		GfOut("Adding Initial Segments...\n");
		PAddSegment(PSegFactory::GetInstance()->CreateSegStr(0, 500.f), pTrack);
		PAddSegment(PSegFactory::GetInstance()->CreateSegCnr(1, PCornerType::CTLeft, 90.f, 0.f, 0.f, 1.5f), pTrack);
		PAddSegment(PSegFactory::GetInstance()->CreateSegStr(2, 500.f), pTrack);

		// Generate Initial 3D Description
		if (!raceOnConsole)
			GenerateTrack(pTrack->trackCache, pTrack->trackCache->params, (char*)pTrack->GetACPathAndName(), nullptr, 0, 0, 0);

		return pTrack;
	}

	tTrack* PGenerateInitialTrack(const char *const fName)
	{
		tTrack* trk = new tTrack();
		char* fileName = new char[strlen(GetLocalDir()) + strlen(fName)];
		strcpy(fileName, GetLocalDir());
		strcat(fileName, fName);
		trk = TrackBuildv1(const_cast<char*>(fName)); // Load in track details, should return with no segments;
		return trk;
	}

	void PGenerateNewSegment(PTrack* track)
	{
		// Create single segment track using last segment on track given
		tTrack* trk = new tTrack();

		// Generate initial track information
		trk = PGenerateInitialTrack(track->GetXMLPathAndName());

		trk->seg = new tTrackSeg(*track->GetEnd());
		
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