/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 05/11/2013
*/

#include "torcsAdaptive.h"

namespace torcsAdaptive
{
	static tTrack* taTrack;
	static int segsAdded;

	// ----- ta Functions ------
	int TaInitTrack(tRmInfo* ReInfo, int trkLength)
	{
		GfOut("\n\n-------------------------------------\n");
		GfOut("INITIALIZING TORCS-ADAPTIVE TRACK...");
		GfOut("\n-------------------------------------\n");

		ReInfo->track = new tTrack();
		taTrack = ReInfo->track;

		// Main info
		GfOut("Assigning Main Track Info...\n");
		taTrack->author = "Torcs-Adaptive";
		taTrack->category = "Adaptive Track";
		taTrack->filename = "AdaptiveTrack";
		taTrack->internalname = "AdaptiveTrack";
		taTrack->name = "Adaptive Track";

		// Length and segment memory allocation
		GfOut("Allocating Segment Memory...\n");
		taTrack->length = trkLength;
		taTrack->nseg = abs(trkLength/LENGTH_PER_SEG);
		taTrack->seg = new tTrackSeg[taTrack->nseg];

		// Add Initial Segments
		GfOut("Adding Initial Segments...\n");
		segsAdded = 0;
		TaAddSegment(taSeg(TR_START, TR_STR, segsAdded, TR_MAIN, 0));
		for(int i = 0; i < (taTrack->nseg - 2); i++)
			TaAddSegment(taSeg(TR_NORMAL, TR_STR, segsAdded, TR_MAIN, 0));
		TaAddSegment(taSeg(TR_LAST, TR_STR, segsAdded, TR_MAIN, 0));

		// Ensure segment pointers loop
		taTrack->seg[taTrack->nseg-1].next = &taTrack->seg[0];
		taTrack->seg[0].prev = &taTrack->seg[taTrack->nseg-1];

		return 0;
	}

	void TaShutDown()
	{
		if(perfMeasurement)
			delete perfMeasurement;
	}

	void TaAddSegment(taSeg seg)
	{
		if(segsAdded <= taTrack->nseg)
		{
			// Create new Seg Object
			taTrack->seg[segsAdded] = tTrackSeg();

			// Obtain Pointer to Segment
			tTrackSeg* curSeg = &(taTrack->seg[segsAdded]);

			// Next and Prev Pointers
			if(segsAdded != 0)
				curSeg->prev = &taTrack->seg[segsAdded-1];
			else
				curSeg->prev = NULL;
			curSeg->next = &taTrack->seg[segsAdded+1];

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

			segsAdded++;

			GfOut("\t- Added Segment ");
			GfOut(curSeg->name);
			GfOut("\n");
		}
		else
			GfOut("\tCannot add new track segment, track is full!\n");
	}


}