/*
	File: k1999Pathfinding.cpp
	Original Author: Remi Coulom
	Modified by: Henri Keeble
	Desc: Pathfinding functions from the TORCS robot K1999 by Remi Coulom, also used in the Berniw robot.
*/

#include "PPathfinder.h"

namespace procBot
{
	/* computes curvature, from Remi Coulom, K1999.cpp */
	inline double PPathfinder::curvature(double xp, double yp, double x, double y, double xn, double yn)
	{
		double x1 = xn - x;
		double y1 = yn - y;
		double x2 = xp - x;
		double y2 = yp - y;
		double x3 = xn - xp;
		double y3 = yn - yp;

		double det = x1 * y2 - x2 * y1;
		double n1 = x1 * x1 + y1 * y1;
		double n2 = x2 * x2 + y2 * y2;
		double n3 = x3 * x3 + y3 * y3;
		double nnn = sqrt(n1 * n2 * n3);
		return 2 * det / nnn;
	}


	/* optimize point p ala k1999 (curvature), Remi Coulom, K1999.cpp */
	inline void PPathfinder::adjustRadius(int s, int p, int e, double c, double security) {
		const double sidedistext = 2.0;
		const double sidedistint = 1.2;

		PTrackSegment* t = track->getSegmentPtr(p);
		v3d *rgh = t->getToRight();
		v3d *left = t->getLeftBorder();
		v3d *right = t->getRightBorder();
		v3d *rs = ps(s)->getLoc(), *rp = ps(p)->getLoc(), *re = ps(e)->getLoc(), n;
		double oldlane = track->distToMiddle(p, rp) / t->getWidth() + 0.5;

		double rgx = (re->x - rs->x), rgy = (re->y - rs->y);
		double m = (rs->x * rgy + rgx * rp->y - rs->y * rgx - rp->x * rgy) / (rgy * rgh->x - rgx * rgh->y);

		n = (*rp) + (*rgh)*m;
		ps(p)->setLoc(&n);
		double newlane = track->distToMiddle(p, rp) / t->getWidth() + 0.5;

		/* get an estimate how much the curvature changes by moving the point 1/10000 of track width */
		const double delta = 0.0001;
		double dx = delta * (right->x - left->x);
		double dy = delta * (right->y - left->y);
		double deltacurvature = curvature(rs->x, rs->y, rp->x + dx, rp->y + dy, re->x, re->y);

		if (deltacurvature > 0.000000001) {
			newlane += (delta / deltacurvature) * c;
			double ExtLane = (sidedistext + security) / t->getWidth();
			double IntLane = (sidedistint + security) / t->getWidth();

			if (ExtLane > 0.5) ExtLane = 0.5;
			if (IntLane > 0.5) IntLane = 0.5;

			if (c >= 0.0) {
				if (newlane < IntLane) newlane = IntLane;
				if (1 - newlane < ExtLane) {
					if (1 - oldlane < ExtLane) newlane = MIN(oldlane, newlane);
					else newlane = 1 - ExtLane;
				}
			}
			else {
				if (newlane < ExtLane) {
					if (oldlane < ExtLane) newlane = MAX(oldlane, newlane);
					else newlane = ExtLane;
				}
				if (1 - newlane < IntLane) newlane = 1 - IntLane;
			}

			double d = (newlane - 0.5) * t->getWidth();
			v3d* trackmiddle = t->getMiddle();

			n = (*trackmiddle) + (*rgh)*d;
			ps(p)->setLoc(&n);
		}
	}


	/* interpolation step from Remi Coulom, K1999.cpp */
	void PPathfinder::stepInterpolate(int iMin, int iMax, int Step)
	{
		int next = (iMax + Step) % ps.Count();
		if (next > ps.Count() - Step) next = 0;

		int prev = (((ps.Count() + iMin - Step) % ps.Count()) / Step) * Step;
		if (prev > ps.Count() - Step)
			prev -= Step;

		v3d *pp = ps(prev)->getLoc();
		v3d *p = ps(iMin)->getLoc();
		v3d *pn = ps(iMax % ps.Count())->getLoc();
		v3d *pnn = ps(next)->getLoc();

		double ir0 = curvature(pp->x, pp->y, p->x, p->y, pn->x, pn->y);
		double ir1 = curvature(p->x, p->y, pn->x, pn->y, pnn->x, pnn->y);

		for (int k = iMax; --k > iMin;) {
			double x = double(k - iMin) / double(iMax - iMin);
			double TargetRInverse = x * ir1 + (1 - x) * ir0;
			adjustRadius(iMin, k, iMax % ps.Count(), TargetRInverse, 0.0);
		}
	}


	/* interpolating from Remi Coulom, K1999.cpp */
	void PPathfinder::interpolate(int step)
	{
		if (step > 1) {
			int i;
			for (i = step; i <= ps.Count() - step; i += step) stepInterpolate(i - step, i, step);
			stepInterpolate(i - step, ps.Count(), step);
		}
	}


	/* smoothing from Remi Coulom, K1999.cpp */
	void PPathfinder::smooth(int Step)
	{
		int prev = ((ps.Count() - Step) / Step) * Step;
		int prevprev = prev - Step;
		int next = Step;
		int nextnext = next + Step;

		v3d *pp, *p, *n, *nn, *cp;

		for (int i = 0; i <= ps.Count() - Step; i += Step) {
			pp = ps(prevprev)->getLoc();
			p = ps(prev)->getLoc();
			cp = ps(i)->getLoc();
			n = ps(next)->getLoc();
			nn = ps(nextnext)->getLoc();

			double ir0 = curvature(pp->x, pp->y, p->x, p->y, cp->x, cp->y);
			double ir1 = curvature(cp->x, cp->y, n->x, n->y, nn->x, nn->y);
			double dx, dy;
			dx = cp->x - p->x; dy = cp->y - p->y;
			double lPrev = sqrt(dx*dx + dy*dy);
			dx = cp->x - n->x; dy = cp->y - n->y;
			double lNext = sqrt(dx*dx + dy*dy);

			double TargetRInverse = (lNext * ir0 + lPrev * ir1) / (lNext + lPrev);

			double Security = lPrev * lNext / (8.0 * 100.0);
			adjustRadius(prev, i, next, TargetRInverse, Security);

			prevprev = prev;
			prev = i;
			next = nextnext;
			nextnext = next + Step;
			if (nextnext > ps.Count() - Step) nextnext = 0;
		}
	}
}