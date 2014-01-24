#include "torcsAdaptive.h"
#include "procedural\pSeg.h"
#include "raceman.h"
#include "trackgen\trackgen.h"
#include "procedural\pTrack.h"
#include "robottools.h"
#include <zlib.h>

#define taOut(out) printf("ta >> " out)

using namespace procedural;

namespace torcsAdaptive
{
	pSegFactory* segFactory = pSegFactory::GetInstance();

	pCarData carData = pCarData();

	void InitCarData(tCarElt* car)
	{
		carData = pCarData(car);
	}

	void AddSegment(tRmInfo* ReInfo, const pSeg& segment)
	{
		taOut("Adding new Segment....\n");

		// Obtain pointers to neccesary data
		pTrack*		  atrack			= ReInfo->_reTrackItf.PGetTrackInfo();
		const char *const acName		= atrack->GetACName();
		const char *const acNameAndPath = atrack->GetACPathAndName();

		// Add Segment
		taOut("\tAdding segment to track.\n");
		ReInfo->_reTrackItf.PAddSegment(segment, atrack);
		taOut("New segment added.\n");
	}
	
	/* Potentially could be used to append AC file where neccesary */
	void UpdateACFile(tRmInfo* ReInfo, pTrack* trInfo)
	{
		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		ReInfo->_reGraphicItf.pDetach3DDesc(trInfo->GetTrackDesc()); // Detach existing description from scene graph

		taOut("\tGenerating new 3D Description.\n");
		GenerateTrack(ReInfo->track, ReInfo->track->params, (char*)trInfo->GetACPathAndName(), NULL, NULL, NULL, 0); // Generate new 3d desc
		trInfo->SetTrackDesc(ReInfo->_reGraphicItf.pLoad3DDesc(trInfo->GetACName(), (ssgLoaderOptions*)trInfo->GetLoaderOptions()));
		ReInfo->_reGraphicItf.pAttach3DDesc(trInfo->GetTrackDesc());
	}

	void UpdateTrack(tRmInfo* ReInfo)
	{
		bool updateAC = false;
		pTrack* trInfo;

		// Update data on the car
		carData.Update();

		if (carData.DirOfTravel() == DirectionOfTravel::BACKWARD)
			taOut("BACKWARD!?\n");

		// Retrieve current procedural track info
		trInfo = ReInfo->_reTrackItf.PGetTrackInfo();
		if (trInfo == nullptr)
			taOut("Error getting procedural track info!\n");

		// If car is on last segment and travelling forward, remove one at the start
		if (carData.CurrentSeg()->id == trInfo->GetEnd()->id)
		{
			if (carData.DirOfTravel() == DirectionOfTravel::FORWARD)
			{
				trInfo->RemoveSegAtStart();
				updateAC = true;
			}
		}

		// Else if car travelling backwards and on first segment, remove a segment at the end. If car on first segment generated, do nothing (THIS NEEDS ANOTHER SOLUTION!)
		else if (carData.CurrentSeg()->id == trInfo->GetStart()->id && carData.CurrentSeg()->id != 0)
		{
			if (carData.DirOfTravel() == DirectionOfTravel::BACKWARD)
			{
				trInfo->RemoveSegAtEnd();
				updateAC = true;
			}
		}
		
		std::cout << trInfo->GetStart()->id << std::endl;
		std::cout << carData.CurrentSeg()->id << std::endl;

		// Determine if another segment needs to be added to the cache and/or track
		if (trInfo->SEG_MEMORY_SIZE > trInfo->trackCache->nseg)
		{
			switch (carData.DirOfTravel())
			{
			case DirectionOfTravel::FORWARD:
				// Determine if new segment needs to be generated
				if (trInfo->trackCache->seg->id == trInfo->segs.End()->id)
					AddSegment(ReInfo, segFactory->CreateSegStr(trInfo->state.curSegIndex, 200.f)); // Generate new segment
				else
					trInfo->AddSegmentAtEnd(); // Add existing segment

				updateAC = true;
				break;

			case DirectionOfTravel::BACKWARD:
				if (carData.CurrentSeg()->id > 0) // Only add new segment if player is not at the start
				{
					trInfo->AddSegmentAtStart();
					updateAC = true;
				}
				break;
			default:
				taOut("Unexpected direction of travel!\n");
				break;
			}
		}

		// Update AC File if necessary
		if (updateAC)
			UpdateACFile(ReInfo, trInfo);
	}

	tdble PercentDistanceToEnd(tdble segLength, tdble distToStart)
	{
		tdble percent = segLength / 100; // Equal to one percent of the segment
		return 100 - (distToStart / percent); // Calculate percent distance from start, take from 100 to find percent distance to end
	}
}