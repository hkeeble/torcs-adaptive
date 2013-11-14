#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#include "perfMeasurement\perfMeasurement.h"
#include "torcsAdaptive\taSeg.h"

namespace torcsAdaptive
{
	#define TA_ADAPTIVE_MAN_NAME "Adaptive Race"
	#define TA_TR_LENGTH 200

	extern bool taAdaptiveMode;
	extern taPerfMeasurement* perfMeasurement;

	void AddSegment(tTrack* track, const taSeg& segment);
	void UpdateACFile(tTrack* track, const char* fName);
}

#endif // _TORCS_ADAPTIVE_H_