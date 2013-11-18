#ifndef _TA_TRACK_STATE_H_
#define _TA_TRACK_STATE_H_

#include "track.h"

namespace torcsAdaptive
{
	/* Used to store the current state of the track for torcs-adaptive */
	struct taTrackState
	{
		taTrackState() { segsSinceLastUpdate = curSegIndex = envIndex = totLength = wi2 = xr = yr = xl = yl = radius = 0.f; };
		int	curSegIndex, envIndex, segsSinceLastUpdate;
		tdble totLength, wi2, xr, yr, xl, yl, radius;
	};
}
#endif // _TA_TRACK_STATE_H_