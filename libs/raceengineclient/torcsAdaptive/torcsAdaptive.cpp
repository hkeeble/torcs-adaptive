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

	int i = 0;

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
		taOut("New segment added.\n");
	}
	
	/* Potentially could be used to append AC file where neccesary */
	void UpdateACFile(tRmInfo* ReInfo, taTrack* trInfo)
	{
		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		ReInfo->_reGraphicItf.taDetach3DDesc(trInfo->GetTrackDesc()); // Detach existing description from scene graph

		taOut("\tGenerating new 3D Description.\n");
		GenerateTrack(ReInfo->track, ReInfo->track->params, (char*)trInfo->GetACPathAndName(), NULL, NULL, NULL, 0); // Generate new 3d desc
		trInfo->SetTrackDesc(ReInfo->_reGraphicItf.taLoad3DDesc(trInfo->GetACName(), (ssgLoaderOptions*)trInfo->GetLoaderOptions()));
		ReInfo->_reGraphicItf.taAttach3DDesc(trInfo->GetTrackDesc());
	}

	void UpdateTrack(tRmInfo* ReInfo)
	{
		bool updateAC = false;
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

		if (i == 0)
		{
			trInfo->RemoveSegAtEnd();
			trInfo->RemoveSegAtEnd();
			trInfo->AddSegmentAtEnd();
			trInfo->AddSegmentAtEnd();
			UpdateACFile(ReInfo, trInfo);
			i = 1;
		}
		//if (curSeg->id + trInfo->SEG_MEMORY_SIZE > trInfo->track->nseg)
		//{
		//	AddSegment(ReInfo, segFactory->CreateSegStr(trInfo->state.curSegIndex, 200.f));
		//	updateAC = true;
		//}
		//
		//if (trInfo->track->nseg > trInfo->SEG_MEMORY_SIZE)
		//{
		//	trInfo->RemoveSegAtStart();
		//	updateAC = true;
		//}

		//if (curSeg->id == 1)
		//{
		//	trInfo->RemoveSegAtStart();
		//	updateAC = true;
		//}

		//// Update AC File if necessary
		//if (updateAC)
		//	UpdateACFile(ReInfo, trInfo);
	}
}