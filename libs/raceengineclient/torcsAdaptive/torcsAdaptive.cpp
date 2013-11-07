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
	
	// Track accessors for ease of reading
	#define taGraphicInfo taTrack->graphic
	#define taSegments taTrack->seg
	#define taSurfaces taTrack->surfaces
	#define taPits taTrack->pits

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
		taTrack->nseg = abs(trkLength/TA_LENGTH_PER_SEG);
		taTrack->seg = new tTrackSeg[taTrack->nseg];

		// Handle Pits
		TaInitPits();

		// Surface List
		TaInitSurfaces();

		// Add Initial Segments
		GfOut("Adding Initial Segments...\n");
		segsAdded = 0;
		TaAddSegment(taSeg(TR_START, TR_STR, segsAdded, TR_MAIN, 0));
		for(int i = 0; i < (taTrack->nseg - 2); i++)
			TaAddSegment(taSeg(TR_NORMAL, TR_STR, segsAdded, TR_MAIN, 0));
		TaAddSegment(taSeg(TR_LAST, TR_STR, segsAdded, TR_MAIN, 0));

		// Ensure segment pointers loop
		taSegments[taTrack->nseg-1].next = &taSegments[0];
		taSegments[0].prev = &taSegments[taTrack->nseg-1];

		return 0;
	}

	void TaShutDown()
	{
		if(perfMeasurement)
			delete perfMeasurement;
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

			segsAdded++;

			GfOut("\t- Added Segment ");
			GfOut(curSeg->name);
			GfOut("\n");
		}
		else
			GfOut("\tCannot add new track segment, track is full!\n");
	}

	void TaInitPits()
	{
		GfOut("Initializing Pits...\n");
		taTrack->pits = tTrackPitInfo(); // Should default all pit values to 0 - pits don't exist

		// ADD CODE HERE
	}

	void TaInitSurfaces()
	{
		GfOut("Initializing Surfaces...\n");

		taSurfaces = new tTrackSurface[TA_MAX_SF]; // Allocate more memory here for more surfaces

		// Next pointers
		for(int i = 0; i < TA_MAX_SF-1; i++)
			taSurfaces[i].next = &taSurfaces[i+1];
		taSurfaces[TA_MAX_SF-1].next = NULL;

		// Road Surface
		taSurfaces[TA_SF_INDEX_ROAD].material	= "asphalt";
		taSurfaces[TA_SF_INDEX_ROAD].kFriction	= 1.2f;
		taSurfaces[TA_SF_INDEX_ROAD].kDammage	= 10.0f;
		taSurfaces[TA_SF_INDEX_ROAD].kRebound	= 1.f;
		taSurfaces[TA_SF_INDEX_ROAD].kRollRes	= 0.001f;
		taSurfaces[TA_SF_INDEX_ROAD].kRoughness	= 0.f;
		taSurfaces[TA_SF_INDEX_ROAD].kRoughWaveLen = 6.2f;
		GfOut("\tAdded Surface ");
		GfOut(taSurfaces[TA_SF_INDEX_ROAD].material);
		GfOut("\n");

		// Barrier Surface
		taSurfaces[TA_SF_INDEX_BARRIER].material		= "barrier";
		taSurfaces[TA_SF_INDEX_BARRIER].kFriction	= 0.f;
		taSurfaces[TA_SF_INDEX_BARRIER].kDammage		= 10.f;
		taSurfaces[TA_SF_INDEX_BARRIER].kRebound		= 1.f;
		taSurfaces[TA_SF_INDEX_BARRIER].kRollRes		= 0.001f;
		taSurfaces[TA_SF_INDEX_BARRIER].kRoughness	= 0.01f;
		taSurfaces[TA_SF_INDEX_BARRIER].kRoughWaveLen = 1.57f;
		GfOut("\tAdded Surface ");
		GfOut(taSurfaces[TA_SF_INDEX_BARRIER].material);
		GfOut("\n");
	}

	void TaInitGraphicInfo()
	{
		GfOut("Initializing Track Graphical Info...\n");
		
		taGraphicInfo = tTrackGraphicInfo(); // Initialize
		taGraphicInfo.background = "background.png";
		taGraphicInfo.background2 = " ";
		taGraphicInfo.bgtype; // ?
		taGraphicInfo.bgColor; // ?
		taGraphicInfo.envnb = 0; // Number of environments?
		taGraphicInfo.env; // char** to name of map, why is this double indirect?
		taGraphicInfo.turnMarksInfo; // ? Is this an array or single object?
	}
}