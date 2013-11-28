#ifndef _TA_TRACK_STATE_H_
#define _TA_TRACK_STATE_H_

namespace torcsAdaptive
{
	/* Used to store the current state of the track for torcs-adaptive */
	struct taTrackState
	{
		taTrackState();
		int	curSegIndex, envIndex, segsSinceLastUpdate;
		float totLength, wi2, xr, yr, xl, yl, radius;
	};
}
#endif // _TA_TRACK_STATE_H_