/*
	File: k1999Pathfinding.cpp
	Original Author: Remi Coulom
	Modified by: Henri Keeble
	Desc: Pathfinding functions from the TORCS robot K1999 by Remi Coulom, also used in the Berniw robot.
*/

#include "K1999.h"

namespace procPathfinder
{
	K1999::K1999(PTrackDesc* track) :
		PPathfinder(track),
		SEGMENT_RANGE(4)
	{
		currentSegID = 0;
	}

	K1999::~K1999()
	{
		// Nothing yet
	}

	void K1999::Plan(PCarDesc* myc)
	{
		double r, length, speedsqr;
		int u, v, w;
		v3d dir;
		int i;

		// Calculate the range that needs to be planned
		int firstID = 0; // ID of the first path segment in the range
		int segCount = 0; // Number of path segments in the range

		path = std::vector<PathSeg>(); // The segments in the path

		currentSegID = track->getSegmentPtr(track->getCurrentSegment(myc->getCarPtr()))->getTrackSegment()->id;

		// Construct a path segment collection of the correct size
		if (currentSegID >= SEGMENT_RANGE+1)
		{
			tdble segsAhead = ceil((tdble)SEGMENT_RANGE / 2.0f);
			tdble segsBehind = SEGMENT_RANGE - segsAhead;

			// Loop to find first segment ID
			PTrackSegment* trkSeg = track->getSegmentPtr(0);
			i = 0;
			while (trkSeg->getTrackSegment()->id != currentSegID - segsBehind) {
				i++;
				trkSeg = track->getSegmentPtr(i);
			}

			firstID = i;

			// Find the number of segments
			while (trkSeg->getTrackSegment()->id != currentSegID + segsAhead) {
				segCount++;
				trkSeg = track->getSegmentPtr(firstID + segCount);
			}

			path = std::vector<PathSeg>(segCount);
			std::cout << segCount << std::endl;
		}
		else
			path = std::vector<PathSeg>(track->segmentCount());

		/* Initialize location to center of the given segment */
		for (i = 0; i < path.size(); i++) {
			path[i].setLoc(track->getSegmentPtr(firstID+i)->getMiddle());
		}

		/* compute path */
		for (int step = 128; (step /= 2) > 0;) {
			for (int i = 100 * int(sqrt((double)step)); --i >= 0;) smooth(step);
			interpolate(step);
		}

		/* init optimal path */
		for (i = 0; i < path.size(); i++) {
			path[i].setOptLoc(path[i].getLoc());
			path[i].setPitLoc(path[i].getOptLoc());
		}

		/* compute possible speeds, direction vector and length of trajectoies */
		u = path.size() - 1; v = 0; w = 1;

		for (i = 0; i < path.size(); i++)
		{
			r = radius(path[u].getLoc()->x, path[u].getLoc()->y,
						path[v].getLoc()->x, path[v].getLoc()->y,
						path[w].getLoc()->x, path[w].getLoc()->y);

			path[i].setRadius(r);

			r = fabs(r);

			tdble mu = track->getSegmentPtr(i)->getKfriction()*myc->CFRICTION*track->getSegmentPtr(i)->getKalpha();
			tdble b = track->getSegmentPtr(i)->getKbeta();

			speedsqr = myc->SPEEDSQRFACTOR*r*g*mu / (1.0 - MIN(1.0, (mu*myc->ca*r / myc->mass)) + mu*r*b);
			
			double maxSpeed = myc->getMaxSpeed();
			double maxSpeedSqr = maxSpeed*maxSpeed;

			if (speedsqr > maxSpeedSqr) {
				speedsqr = maxSpeedSqr;
			}

			dir = *path[w].getLoc() - *path[u].getLoc();
			length = dist(path[v].getLoc(), path[w].getLoc());

			dir.normalize();

			path[i].set(speedsqr, length, &dir);

			u = v; v = w; w = (w + 1 + path.size()) % path.size();
		}

		// Insert calculated path into segment collection
		ps.Insert(path, firstID);

		// Record the PS count on this call
		previousPSCount = ps.Count();
	}

	/* computes curvature, from Remi Coulom, K1999.cpp */
	inline double K1999::curvature(double xp, double yp, double x, double y, double xn, double yn)
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
	inline void K1999::adjustRadius(int s, int p, int e, double c, double security) {
		const double sidedistext = 2.0;
		const double sidedistint = 1.2;

		PTrackSegment* t = track->getSegmentPtr(p);
		v3d *rgh = t->getToRight();
		v3d *left = t->getLeftBorder();
		v3d *right = t->getRightBorder();
		v3d *rs = path[s].getLoc(), *rp = path[p].getLoc(), *re = path[e].getLoc(), n;
		double oldlane = track->distToMiddle(p, rp) / t->getWidth() + 0.5;

		double rgx = (re->x - rs->x), rgy = (re->y - rs->y);
		double m = (rs->x * rgy + rgx * rp->y - rs->y * rgx - rp->x * rgy) / (rgy * rgh->x - rgx * rgh->y);

		n = (*rp) + (*rgh)*m;
		path[p].setLoc(&n);
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
			path[p].setLoc(&n);
		}
	}


	/* interpolation step from Remi Coulom, K1999.cpp */
	void K1999::stepInterpolate(int iMin, int iMax, int Step)
	{
		int next = (iMax + Step) % path.size();
		if (next > path.size() - Step) next = 0;

		int prev = (((path.size() + iMin - Step) % path.size()) / Step) * Step;
		if (prev > path.size() - Step)
			prev -= Step;

		v3d *pp = path[prev].getLoc();
		v3d *p = path[iMin].getLoc();
		v3d *pn = path[iMax % path.size()].getLoc();
		v3d *pnn = path[next].getLoc();

		double ir0 = curvature(pp->x, pp->y, p->x, p->y, pn->x, pn->y);
		double ir1 = curvature(p->x, p->y, pn->x, pn->y, pnn->x, pnn->y);

		for (int k = iMax; --k > iMin;) {
			double x = double(k - iMin) / double(iMax - iMin);
			double TargetRInverse = x * ir1 + (1 - x) * ir0;
			adjustRadius(iMin, k, iMax % path.size(), TargetRInverse, 0.0);
		}
	}


	/* interpolating from Remi Coulom, K1999.cpp */
	void K1999::interpolate(int step)
	{
		if (step > 1) {
			int i;
			for (i = step; i <= path.size() - step; i += step) stepInterpolate(i - step, i, step);
			stepInterpolate(i - step, path.size(), step);
		}
	}


	/* smoothing from Remi Coulom, K1999.cpp */
	void K1999::smooth(int Step)
	{
		int prev = ((path.size() - Step) / Step) * Step;
		int prevprev = prev - Step;
		int next = Step;
		int nextnext = next + Step;

		v3d *pp, *p, *n, *nn, *cp;

		for (int i = 0; i <= path.size() - Step; i += Step) {
			pp = path[prevprev].getLoc();
			p = path[prev].getLoc();
			cp = path[i].getLoc();
			n = path[next].getLoc();
			nn = path[nextnext].getLoc();

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
			if (nextnext > path.size() - Step) nextnext = 0;
		}
	}
}