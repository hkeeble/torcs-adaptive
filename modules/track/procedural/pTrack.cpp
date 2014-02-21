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
		pTrack->trk = BuildTrack(pTrack->GetXMLPathAndName());

		// Initialize Sides
		InitSides(pTrack->trk->params, pTrack->trk);

		// Add Initial Segments
		GfOut("Adding Initial Segments...\n");
		PAddSegment(PSegFactory::GetInstance()->CreateRandomStr(0), pTrack);

		//for (int i = 0; i < INITIAL_SEG_GEN; i++)
			//PAddSegment(PSegFactory::GetInstance()->CreateRandomSeg(i), pTrack);

		// Generate Initial 3D Description
		if (!raceOnConsole)
			GenerateTrack(pTrack->trk, pTrack->trk->params, (char*)pTrack->GetACPathAndName(), nullptr, 0, 0, 0);

		return pTrack;
	}

	tTrack* BuildTrack(const char *const fName)
	{
		tTrack* track = (tTrack*)calloc(1, sizeof(tTrack));
		void* trackHandle;
		tRoadCam* theCamList = (tRoadCam*)NULL;

		track->params = trackHandle = GfParmReadFile(fName, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);

		track->filename = strdup(fName);

		GetTrackHeader(trackHandle, track);

		switch (track->version) {
		case 0:
		case 1:
		case 2:
		case 3:
			ReadTrack3(track, trackHandle, &theCamList, 0);
			break;
		case 4:
			ReadTrack4(track, trackHandle, &theCamList, 0);
			break;
		}

		return track;
	}

	void PUpdateACFile(PTrack* track)
	{
		// Create single segment track using last segment on track given
		tTrack* trk = new tTrack();

		// Generate initial track information
		trk = BuildTrack(track->GetXMLPathAndName());

		// Initialize a single segment as the last segment in the given track
		trk->seg = new tTrackSeg(*track->GetEnd());
		
		// Break links to actual track
		trk->seg->prev = trk->seg;
		trk->seg->next = trk->seg;

		// Manually set some parameters used by track generator
		trk->nseg = 1;
		trk->length = trk->seg->length;
		
		// Initialize Sides
		InitSides(trk->params, trk);

		// Generate 3D description in temporary file
		GenerateTrack(trk, trk->params, (char*)track->GetTempACPathAndName(), nullptr, 0, 0, 0);

		// Update track's AC file
		track->UpdateACFile(trk->seg->id);
	}

	/* Release all torcs-adaptive track module resources */
	void PShutDown(PTrack* track)
	{
		TrackShutdown();
	}
}