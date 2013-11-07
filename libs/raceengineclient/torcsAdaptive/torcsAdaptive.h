#ifndef _TORCS_ADAPTIVE_H_C
#define _TORCS_ADAPTIVE_H_C

#include "perfMeasurement\perfMeasurement.h"

namespace torcsAdaptive
{
	#define TA_ADAPTIVE_MAN_NAME "Adaptive Race"
	#define TA_TR_LENGTH 200

	extern bool taAdaptiveMode;
	extern taPerfMeasurement* perfMeasurement;
}

#endif // _TORCS_ADAPTIVE_H_C