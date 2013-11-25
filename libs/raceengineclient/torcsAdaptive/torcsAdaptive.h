#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#include "perfMeasurement\perfMeasurement.h"
#include "torcsAdaptive\taSeg.h"
#include "raceman.h"

namespace torcsAdaptive
{
	#define TA_ADAPTIVE_MAN_NAME "Adaptive Race"
	#define TA_TR_LENGTH 200

	extern bool taAdaptiveMode;
	extern taPerfMeasurement* perfMeasurement;

	void AddSegment(tRmInfo* ReInfo, const taSeg& segment);
	void UpdateACFile(taTrackState state, tTrack* track, const char* fName);
	void UpdateTrack(tRmInfo* ReInfo);
}

#endif // _TORCS_ADAPTIVE_H_