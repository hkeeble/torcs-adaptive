#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#include "perfMeasurement\perfMeasurement.h"
#include "procedural\pSeg.h"
#include "procedural\pCarData.h"
#include "raceman.h"

using namespace procedural;

namespace torcsAdaptive
{
	#define TA_ADAPTIVE_MAN_NAME "Adaptive Race"
	#define TA_TR_LENGTH 200

	/* Designates if TORCS is playing an adaptive race or not. */
	extern bool taAdaptiveMode;

	/* The performance measurement object. */
	extern taPerfMeasurement* perfMeasurement;
	
	/* A segment factory used to easily instantiate different types of procedural segments. */
	extern pSegFactory* segFactory;

	/* Used to track current car data */
	extern pCarData carData;

	/* Initialize car data to be tracked for procedural generation. */
	void InitCarData(tCarElt* car);

	/* Add a given segment to the track contained in the race manager passed in. */
	void AddSegment(tRmInfo* ReInfo, const pSeg& segment);

	/* Update the AC File of a track contained within the race manaegr passed in. Also requires procedural track information. */
	void UpdateACFile(tRmInfo* ReInfo, pTrack* trInfo);

	/* Update a procedural track, managing it's cache and adding segments if neccesary. */
	void UpdateTrack(tRmInfo* ReInfo);

	/* Calculate percentage of segment left until the end */
	tdble PercentDistanceToEnd(tdble segLength, tdble distToStart);
}

#endif // _TORCS_ADAPTIVE_H_