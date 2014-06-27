#include "PPathfinder.h"

#ifndef _K1999_H_
#define _K1999_H_

namespace procPathfinder
{
	class K1999 : public PPathfinder
	{
	public:
		K1999(PTrackDesc* itrack, PCarDesc* carDesc, tSituation *situation);
		~K1999();

		/*!< Plan the path. Override in base classes. */
		void Plan(PCarDesc* myc) override final;

	private:
		double curvature(double xp, double yp, double x, double y, double xn, double yn);
		void adjustRadius(int s, int p, int e, double c, double carwidth);
		void stepInterpolate(int iMin, int iMax, int Step);
		void interpolate(int Step);
		void smooth(int Step);
	};
}

#endif // _K1999_H_