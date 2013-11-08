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
		GfOut("Adding Initial Segments...\n");
		segsAdded = 0;
		TaAddSegment(taSeg(TR_START, TR_STR, segsAdded, TR_MAIN, 0));
		for(int i = 0; i < (taTrack->nseg - 2); i++)
			TaAddSegment(taSeg(TR_NORMAL, TR_STR, segsAdded, TR_MAIN, 0));
		TaAddSegment(taSeg(TR_LAST, TR_STR, segsAdded, TR_MAIN, 0));

		return taTrack;
	}

	void TaShutDown()
	{
		if(taTrack)
			delete taTrack;
	}

	void TaAddSegment(taSeg seg)
	{
		if(segsAdded <= taTrack->nseg)
		{
			// Create new Seg Object
			taSegments[segsAdded] = tTrackSeg();

			// Obtain Pointer to Segment
			tTrackSeg* curSeg = &(taSegments[segsAdded]);

			// Next and Prev Pointers
			if(segsAdded != 0)
				curSeg->prev = &taSegments[segsAdded-1];
			else
				curSeg->prev = NULL;
			curSeg->next = &taSegments[segsAdded+1];

			// Assign Name (based on ID for debugging)
			std::string n = "ID" + std::to_string(seg.id);
			curSeg->name = new char[strlen(n.c_str())];
			strcpy((char*)curSeg->name, ("ID" + std::to_string(seg.id)).c_str());

			// Assign Values
			curSeg->raceInfo	=	seg.raceInfo;
			curSeg->type		=	seg.type;
			curSeg->id			=	seg.id;
			curSeg->type2		=	seg.type2;
			curSeg->style		=	seg.style;

			// Data 
			curSeg->length		=	seg.length;
			curSeg->width		=	taTrack->width;
			curSeg->startWidth	=	taTrack->width;
			curSeg->endWidth	=	taTrack->width;

			// For Corners
			curSeg->radius		=	seg.radius;
			curSeg->radiusr		=	seg.radiusr;
			curSeg->radiusl		=	seg.radiusl;
			curSeg->arc			=	seg.arc;

			// Surface
			curSeg->surface		=	&taTrack->surfaces[TA_SF_INDEX_ROAD];

			// Extenstions
			curSeg->ext			=	new tSegExt(); // Initial Values Used

			// Camera
			// ??

			// Height of Curbs
			curSeg->height = 0.f;

			// Constants for Points (Not sure what these are used for)
			curSeg->Kzl = 0.f;
			curSeg->Kzw = 0.f;
			curSeg->Kyl = 0.f;

			// Barriers
			// ??

			// Environment Index
			curSeg->envIndex = 0;

			// DoVFactor (???)
			curSeg->DoVfactor = 0.f;

			// Add Sides
			//AddSides(curSeg, taTrack->params, taTrack, 

			segsAdded++;

			GfOut("\t- Added Segment ");
			GfOut(curSeg->name);
			GfOut("\n");
		}
		else
			GfFatal("\tCannot add new track segment, track is full!\n");
	}
}