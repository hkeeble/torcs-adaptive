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
	static tTrack* taTrack;
	static int segsAdded;
	
	// Track accessors for ease of reading
	#define taGraphicInfo taTrack->graphic
	#define taSegments taTrack->seg
	#define taSurfaces taTrack->surfaces
	#define taPits taTrack->pits

	// ----- ta Functions ------
	tTrack* TaInitTrack(int trkLength)
	{
		const int BUFSIZE = 256;
		char buf[BUFSIZE];

		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		taTrack =  new tTrack(); // Allocate memory for track

		if(!taTrack)
			GfFatal("Error allocating memory for adaptive track!\n");

		// Main info
		GfOut("Assigning Main Track Info...\n");
		char* fName = strcat(GetLocalDir(), "tracks/adaptive/taTrack1/taTrack1.xml");
		taTrack = TrackBuildv1(fName);

		// Length and segment memory allocation
		GfOut("Allocating Segment Memory...\n");
		taTrack->length = trkLength;
		taTrack->nseg = abs(trkLength/TA_LENGTH_PER_SEG);
		taTrack->seg = new tTrackSeg[taTrack->nseg];

		// Add Initial Segments
		//GfOut("Adding Initial Segments...\n");
		//segsAdded = 0;
		//TaAddSegment(taSeg(TR_START, TR_STR, segsAdded, TR_MAIN, 0));
		//for(int i = 0; i < (taTrack->nseg - 2); i++)
		//	TaAddSegment(taSeg(TR_NORMAL, TR_STR, segsAdded, TR_MAIN, 0));
		//TaAddSegment(taSeg(TR_LAST, TR_STR, segsAdded, TR_MAIN, 0));

		return taTrack;
	}

	void TaShutDown()
	{
		if(taTrack)
			delete taTrack;
	}
}