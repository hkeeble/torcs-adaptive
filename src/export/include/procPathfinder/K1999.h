#include "PPathfinder.h"

#ifndef _K1999_H_
#define _K1999_H_

namespace procPathfinder
{
	class K1999 : public PPathfinder
	{
	public:
		K1999(PTrackDesc* track);
		~K1999();

		/*!< Plan the path. Override in base classes. */
		void Plan(PCarDesc* myc) override final;

	private:
		const int SEGMENT_RANGE; /*!< The range of TORCS track segments to use for path calculation. */

		int currentSegID; /*!< The current track segment ID of the car. */

		std::vector<PathSeg> path; /*!< The collection of segments being planned in the current planning run. Used for optimization purposes. */

		double curvature(double xp, double yp, double x, double y, double xn, double yn);
		void adjustRadius(int s, int p, int e, double c, double carwidth);
		void stepInterpolate(int iMin, int iMax, int Step);
		void interpolate(int Step);
		void smooth(int Step);
	};
}

#endif // _K1999_H_