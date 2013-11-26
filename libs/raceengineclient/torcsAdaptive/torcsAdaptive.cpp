#include "torcsAdaptive.h"
#include "torcsAdaptive\taSeg.h"
#include "raceman.h"
#include "trackgen\trackgen.h"
#include <zlib.h>

#define taOut(out) printf("ta >> " out)

namespace torcsAdaptive
{
	void AddSegment(tRmInfo* ReInfo, const taSeg& segment)
	{
		taOut("Adding new Segment....\n");

		// Obtain pointers to neccesary data
		EntityDesc* desc	= ReInfo->_reTrackItf.taGetTrackDesc();
		char* acNameAndPath = ReInfo->_reTrackItf.taGetACNameAndPath();
		char* acName		= ReInfo->_reTrackItf.taGetACName();

		// Add Segment
		taOut("\tAdding segment to track.\n");
		ReInfo->_reTrackItf.taAddSegment(segment, ReInfo->track);

		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		ReInfo->_reGraphicItf.taDetach3DDesc(desc); // Detach existing description from scene graph
		
		taOut("\tGenerating new 3D Description.\n");
		GenerateTrack(ReInfo->track, ReInfo->track->params, acNameAndPath, NULL, NULL, NULL, 0); // Generate new 3d desc
		desc = ReInfo->_reGraphicItf.taLoad3DDesc(ReInfo->_reTrackItf.taGetACName());
		ReInfo->_reTrackItf.taSetTrackDesc(desc);
		ReInfo->_reGraphicItf.taAttach3DDesc(ReInfo->_reTrackItf.taGetTrackDesc());

		taOut("New segment added.\n");
	}
	
	/* Potentially could be used to append AC file where neccesary */
	void UpdateACFile(taTrackState state, tTrack* track, const char* fName)
	{
		FILE* currentFile = fopen(fName, "a");

		if(currentFile == NULL)
			GfFatal("Error! Could not load current track AC file '%900s'.", fName);

		state.segsSinceLastUpdate = 0;
	}

	void UpdateTrack(tRmInfo* ReInfo)
	{
		tTrackSeg* curSeg;
		tCarElt* car;
		tdble curSegLength, distFromStart, segLeft, segPerc;
		curSegLength = distFromStart = segLeft = segPerc = 0;
		curSeg = NULL;
		car = NULL;

		car = (tCarElt*)perfMeasurement->GetCar();
		if(car == NULL)
			taOut("Error getting car from perfMeasurement!\n");

		curSeg = car->priv.wheel[0].seg;
		if(curSeg == NULL)
			taOut("Error getting current segment!\n");

		if(curSeg->next == ReInfo->track->seg)
		{
			curSegLength = curSeg->length;
			if(curSegLength == 0)
				curSegLength = curSeg->arc;
			distFromStart = car->pub.trkPos.toStart;

			segLeft = curSegLength-distFromStart;

			if(segLeft < curSegLength/10)
				AddSegment(ReInfo, taSeg(TR_NORMAL, TR_STR, ReInfo->_reTrackItf.taGetTrackState().curSegIndex, TR_MAIN, 0, 200.f));
		}
	}
}