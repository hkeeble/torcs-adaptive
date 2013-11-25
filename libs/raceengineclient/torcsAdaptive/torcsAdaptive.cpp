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
		
		ReInfo->_reTrackItf.taSetTrackDesc(ReInfo->_reGraphicItf.taLoad3DDesc(ReInfo->_reTrackItf.taGetACName()));
		ReInfo->_reGraphicItf.taAttach3DDesc(ReInfo->_reTrackItf.taGetTrackDesc());

		taOut("New segment added.\n");
	}
	
	void UpdateACFile(taTrackState state, tTrack* track, const char* fName)
	{
		FILE* currentFile = fopen(fName, "a");

		if(currentFile == NULL)
			GfFatal("Error! Could not load current track AC file '%900s'.", fName);

		state.segsSinceLastUpdate = 0;
	}

	void UpdateTrack(tRmInfo* ReInfo)
	{
		tdble curSegLength, distFromStart, segLeft;
		curSegLength = distFromStart = segLeft = 0;

		curSegLength = perfMeasurement->GetCar()->priv.wheel[0].seg->length;
		if(curSegLength == 0)
			curSegLength = perfMeasurement->GetCar()->priv.wheel[0].seg->arc;
		distFromStart = perfMeasurement->GetCar()->pub.trkPos.toStart;

		segLeft = curSegLength-distFromStart;

		if(segLeft > curSegLength/4)
			AddSegment(ReInfo, taSeg(TR_NORMAL, TR_STR, ReInfo->_reTrackItf.taGetTrackState().curSegIndex, TR_MAIN, 0, 200.f));
	}
}