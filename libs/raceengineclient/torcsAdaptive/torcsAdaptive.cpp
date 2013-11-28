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
		taTrackInfo*	  trInfo		= ReInfo->_reTrackItf.taGetTrackInfo();
		const char *const acName		= trInfo->GetACName();
		const char *const acNameAndPath = trInfo->GetACPathAndName();

			// Add Segment
		taOut("\tAdding segment to track.\n");
		ReInfo->_reTrackItf.taAddSegment(segment, ReInfo->track);

		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		ReInfo->_reGraphicItf.taDetach3DDesc(trInfo->GetTrackDesc()); // Detach existing description from scene graph
		
		taOut("\tGenerating new 3D Description.\n");
		GenerateTrack(ReInfo->track, ReInfo->track->params, (char*)acNameAndPath, NULL, NULL, NULL, 0); // Generate new 3d desc
		trInfo->SetTrackDesc(ReInfo->_reGraphicItf.taLoad3DDesc(acName, (ssgLoaderOptions*)trInfo->GetLoaderOptions()));
		ReInfo->_reGraphicItf.taAttach3DDesc(trInfo->GetTrackDesc());

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
		taTrackInfo* trInfo;
		tTrackSeg* curSeg;
		tCarElt* car;
		tdble curSegLength, distFromStart, segLeft, segPerc;
		curSegLength = distFromStart = segLeft = segPerc = 0;
		curSeg = NULL;
		car = NULL;

		car = (tCarElt*)perfMeasurement->GetCar();
		if(car == NULL)
			taOut("Error getting car from perfMeasurement!\n");

		curSeg = car->priv.wheel[3].seg;
		if(curSeg == NULL)
			taOut("Error getting current segment!\n");

		trInfo = ReInfo->_reTrackItf.taGetTrackInfo();
		if(trInfo == NULL)
			taOut("Error getting track info!\n");

		if(curSeg->next == ReInfo->track->seg)
		{
			curSegLength = curSeg->length;
			if(curSegLength == 0)
				curSegLength = curSeg->arc;
			distFromStart = car->pub.trkPos.toStart;

			segPerc = curSegLength/100;
			segLeft = curSegLength-distFromStart;
			segLeft = segLeft/segPerc;

			if(segLeft < 10) // If car is in last 10 percent of a segment, add a new segment
				AddSegment(ReInfo, taSeg(TR_NORMAL, TR_STR, trInfo->state.curSegIndex, TR_MAIN, 0, 200.f));
		}
	}
}