#include "perfMeasurement\K1999RaceLine.h"

namespace perfMeasurement
{
	K1999RaceLine::K1999RaceLine(tTrack* theTrack) : Pathfinder(theTrack)
	{
		// nothing yet
	}

	void K1999RaceLine::Plan()
	{

	}

	void K1999RaceLine::UpdatePlan()
	{
		if (segments.size() < theTrack->nseg)
		{
			for (int i = 0; i < theTrack->nseg - segments.size(); i++)
				segments.push_back(PathSegment(v2d(theTrack->seg[i].center.x, theTrack->seg[i].center.z)));
		}
		Output("optimal.dat");
	}
}