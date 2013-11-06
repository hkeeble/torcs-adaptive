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
		GfOut("Initializing TORCS-Adaptive track...");

		ReInfo->track = new tTrack();
		taTrack = ReInfo->track;

		// Main info
		taTrack->author = "Torcs-Adaptive";
		taTrack->category = "Adaptive Track";
		taTrack->filename = " ";
		taTrack->internalname = "AdaptiveTrack";
		taTrack->name = "Adaptive Track";

		// Length and segment memory allocation
		taTrack->length = trkLength;
		taTrack->nseg = abs(trkLength/LENGTH_PER_SEG);
		taTrack->seg = new tTrackSeg[taTrack->nseg];

		// Add Start Segment
		segsAdded = 0;
		TaAddSegment(taSeg(TR_START, TR_STR, 0, TR_MAIN, 0));

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
			tTrackSeg* curSeg = &taTrack->seg[segsAdded];

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
		}
		else
			GfOut("Cannot add new track segment, track is full!");
	}


}