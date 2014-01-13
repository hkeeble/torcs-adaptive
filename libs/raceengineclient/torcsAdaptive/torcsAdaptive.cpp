#include "torcsAdaptive.h"
#include "torcsAdaptive\taSeg.h"
#include "raceman.h"
#include "trackgen\trackgen.h"
#include "torcsAdaptive\taTrack.h"
#include <zlib.h>

#define taOut(out) printf("ta >> " out)

namespace torcsAdaptive
{
	TaSegFactory* segFactory = TaSegFactory::GetInstance();

	void AddSegment(tRmInfo* ReInfo, const taSeg& segment)
	{
		taOut("Adding new Segment....\n");

		// Obtain pointers to neccesary data
		taTrack*		  atrack		= ReInfo->_reTrackItf.taGetTrackInfo();
		const char *const acName		= atrack->GetACName();
		const char *const acNameAndPath = atrack->GetACPathAndName();

		// Add Segment
		taOut("\tAdding segment to track.\n");
		ReInfo->_reTrackItf.taAddSegment(segment, atrack);

		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		ReInfo->_reGraphicItf.taDetach3DDesc(atrack->GetTrackDesc()); // Detach existing description from scene graph
		
		taOut("\tGenerating new 3D Description.\n");
		GenerateTrack(ReInfo->track, ReInfo->track->params, (char*)acNameAndPath, NULL, NULL, NULL, 0); // Generate new 3d desc
		atrack->SetTrackDesc(ReInfo->_reGraphicItf.taLoad3DDesc(acName, (ssgLoaderOptions*)atrack->GetLoaderOptions()));
		ReInfo->_reGraphicItf.taAttach3DDesc(atrack->GetTrackDesc());

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
		taTrack* trInfo;
		tTrackSeg* curSeg;
		tCarElt* car;
		tdble curSegLength, distFromStart, segLeft, segPerc;
		curSegLength = distFromStart = segLeft = segPerc = 0;
		curSeg = NULL;
		car = NULL;

		car = (tCarElt*)perfMeasurement->GetCar();
		if (car == NULL)
			taOut("Error getting car from perfMeasurement!\n");

		curSeg = car->priv.wheel[3].seg;
		if (curSeg == NULL)
			taOut("Error getting current segment!\n");

		trInfo = ReInfo->_reTrackItf.taGetTrackInfo();
		if (trInfo == NULL)
			taOut("Error getting track info!\n");

		if (curSeg->id + trInfo->SEG_MEMORY_SIZE > trInfo->track->nseg)
			AddSegment(ReInfo, segFactory->CreateSegStr(trInfo->state.curSegIndex, 200.f));

		if (trInfo->track->nseg > trInfo->SEG_MEMORY_SIZE)
			trInfo->RemoveSegAtStart();
	}
}