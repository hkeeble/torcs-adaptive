/*
	File: PPPathfinder.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Pathfinding functions from the berniw robot.
*/

#include "BerniwPathfinding.h"
#include "taMath\spline.h"

namespace procPathfinder
{
	BerniwPathfinding::BerniwPathfinding(PTrackDesc* itrack, PCarDesc* carDesc, tSituation *situation) : PPathfinder(itrack, carDesc, situation)
	{
		// nothing yet
	}

	BerniwPathfinding::~BerniwPathfinding()
	{
		// nothing yet
	}

	void BerniwPathfinding::Plan(PCarDesc* myc)
	{
		double r, length, speedsqr;
		int u, v, w;
		v3d dir;
		int i;

		/* Initialize location to center of the given segment */
		for (i = previousPSCount; i < ps.Count(); i++)
			ps(i)->setLoc(track->getSegmentPtr(i)->getMiddle());

		optimize3(previousPSCount, ps.Count(), 1.0);
		optimize3(previousPSCount + 2, ps.Count(), 1.0);
		optimize3(previousPSCount + 1, ps.Count(), 1.0);

		optimize2(previousPSCount, 10 * ps.Count(), 0.5);
		optimize(previousPSCount, 80 * ps.Count(), 1.0);

		for (int k = 0; k < 10; k++) {
			const int step = 65536 * 64;
			for (int j = previousPSCount; j < ps.Count(); j++) {
				for (int i = step; i > 0; i /= 2) {
					smooth(j, (double)i / (step / 2), myc->CARWIDTH / 2.0);
				}
			}
		}

		/* init optimal path */
		for (i = previousPSCount; i < ps.Count(); i++) {
			ps(i)->setOptLoc(ps(i)->getLoc());
			ps(i)->setPitLoc(ps(i)->getOptLoc());
		}

		/* compute possible speeds, direction vector and length of trajectoies */
		u = ps.Count() - 1; v = 0; w = 1;

		for (i = previousPSCount; i < ps.Count(); i++)
		{
			r = radius(ps(u)->getLoc()->x, ps(u)->getLoc()->y,
				ps(v)->getLoc()->x, ps(v)->getLoc()->y, ps(w)->getLoc()->x, ps(w)->getLoc()->y);
			ps(i)->setRadius(r);
			r = fabs(r);

			length = dist(ps(v)->getLoc(), ps(w)->getLoc());

			tdble mu = track->getSegmentPtr(i)->getKfriction()*myc->CFRICTION*track->getSegmentPtr(i)->getKalpha();
			tdble b = track->getSegmentPtr(i)->getKbeta();
			speedsqr = myc->SPEEDSQRFACTOR*r*g*mu / (1.0 - MIN(1.0, (mu*myc->ca*r / myc->mass)) + mu*r*b);

			dir = *(ps(w)->getLoc()) - (*ps(u)->getLoc());
			dir.normalize();

			//ps(i)->set(speedsqr, length, ps(v)->getLoc(), &dir);
			ps(i)->set(speedsqr, length, &dir);

			u = v; v = w; w = (w + 1 + ps.Count()) % ps.Count();
		}

		// Record the PS count on this call
		previousPSCount = ps.Count();
	}

	/*
	computes int(sin(u^2), u=0..alpha), where alpha is [0..PI).
	*/
	double BerniwPathfinding::intsinsqr(double alpha)
	{
		int i = (int)floor(alpha / TPRES), j = i + 1;
		/* linear interpoation between the nearest known two points */
		return cp[i].is + (alpha - cp[i].x)*(cp[j].is - cp[i].is) / TPRES;
	}


	/*
	computes int(cos(u^2), u=0..alpha), where alpha is [0..PI).
	*/
	double BerniwPathfinding::intcossqr(double alpha)
	{
		int i = (int)floor(alpha / TPRES), j = i + 1;
		/* linear interpoation between the nearest known two points */
		return cp[i].ic + (alpha - cp[i].x)*(cp[j].ic - cp[i].ic) / TPRES;
	}

	double BerniwPathfinding::clothlength(double beta, double y)
	{
		return 2.0*sqrt(2.0*beta / clothsigma(beta, y));
	}

	double BerniwPathfinding::clothsigma(double beta, double y)
	{
		double a = intsinsqr(sqrt(fabs(beta))) / y;
		return a*a*2.0;
	}

	/*
	searches for the startid of a part, eg. TR_STR
	*/

	int BerniwPathfinding::findStartSegId(int id)
	{

		double radius = track->getSegmentPtr(id)->getRadius();
		int type = track->getSegmentPtr(id)->getType();
		int i = (id - 1 + ps.Count()) % ps.Count(), j = id;

		while (track->getSegmentPtr(i)->getType() == type &&
			track->getSegmentPtr(i)->getRadius() == radius &&
			i != id) {
			j = i;
			i = (i - 1 + ps.Count()) % ps.Count();
		}
		return j;
	}


	/*
	searches for the endid of a part, eg. TR_STR
	*/
	int BerniwPathfinding::findEndSegId(int id)
	{
		double radius = track->getSegmentPtr(id)->getRadius();
		int type = track->getSegmentPtr(id)->getType();
		int i = (id + 1 + ps.Count()) % ps.Count(), j = id;

		while (track->getSegmentPtr(i)->getType() == type &&
			track->getSegmentPtr(i)->getRadius() == radius &&
			i != id) {
			j = i;
			i = (i + 1 + ps.Count()) % ps.Count();
		}
		return j;
	}


	/*
	weight function, x 0..1
	*/
	double BerniwPathfinding::computeWeight(double x, double len)
	{
		return (x <= 0.5) ? (2.0*x)*len : (2.0*(1.0 - x))*len;
	}


	/*
	modify point according to the weights
	*/
	void BerniwPathfinding::setLocWeighted(int id, double newweight, v3d* newp)
	{
		double oldweight = ps(id)->getWeight();
		v3d* oldp = ps(id)->getLoc();
		v3d p;

		/* ugly, fix it in init.... */
		if (newweight < 0.001) newweight = 0.001;

		if (oldweight + newweight == 0.0) printf("ops! O: %f, N: %f\n", oldweight, newweight);
		if (oldweight > newweight) {
			double d = newweight / (oldweight + newweight);
			p = (*oldp) + (*newp - *oldp)*d;
			ps(id)->setLoc(&p);
			ps(id)->setWeight(oldweight + newweight);
		}
		else {
			double d = oldweight / (oldweight + newweight);
			p = (*newp) + (*oldp - *newp)*d;
			ps(id)->setLoc(&p);
			ps(id)->setWeight(oldweight + newweight);
		}
	}

	/*
	initializes the path for straight parts of the track.
	*/
	int BerniwPathfinding::initStraight(int id, double w)
	{
		int start = findStartSegId(id), end = findEndSegId(id);
		int prev = (start - 1 + ps.Count()) % ps.Count(), next = (end + 1) % ps.Count();
		int prevtype = track->getSegmentPtr(prev)->getType();
		int nexttype = track->getSegmentPtr(next)->getType();
		int len = track->diffSegId(start, end);

		if (prevtype == nexttype) {
			if (prevtype == TR_RGT) {
				int l = 0;
				for (int i = start; i != next; i++) {
					i = i % ps.Count();
					if (ps(i)->getWeight() == 0.0) {
						v3d* p = track->getSegmentPtr(i)->getLeftBorder();
						v3d* r = track->getSegmentPtr(i)->getToRight();
						v3d np;
						np = (*p) + (*r)*w;
						setLocWeighted(i, computeWeight(((double)l) / ((double)len), len), &np);
						l++;
					}
				}
			}
			else {
				int l = 0;
				for (int i = start; i != next; i++) {
					i = i % ps.Count();
					if (ps(i)->getWeight() == 0.0) {
						v3d* p = track->getSegmentPtr(i)->getRightBorder();
						v3d* r = track->getSegmentPtr(i)->getToRight();
						v3d np;
						np = (*p) - (*r)*w;
						setLocWeighted(i, computeWeight(((double)l) / ((double)len), len), &np);
						l++;
					}
				}
			}
		}
		else {
			double startwidth = track->getSegmentPtr(start)->getWidth() / 2.0 - w;
			double endwidth = track->getSegmentPtr(end)->getWidth() / 2.0 - w;
			double dw = (startwidth + endwidth) / len;
			int l = 0;
			if (prevtype == TR_RGT) {
				for (int i = start; i != next; i++) {
					i = i % ps.Count();
					v3d* p = track->getSegmentPtr(i)->getLeftBorder();
					v3d* r = track->getSegmentPtr(i)->getToRight();
					v3d np;
					np = (*p) + (*r)*(w + dw*l);
					setLocWeighted(i, computeWeight(((double)l) / ((double)len), len), &np);
					l++;
				}
			}
			else {
				for (int i = start; i != next; i++) {
					i = i % ps.Count();
					v3d* p = track->getSegmentPtr(i)->getRightBorder();
					v3d* r = track->getSegmentPtr(i)->getToRight();
					v3d np;
					np = (*p) - (*r)*(w + dw*l);
					setLocWeighted(i, computeWeight(((double)l) / ((double)len), len), &np);
					l++;
				}
			}
		}
		return next;
	}


	/*
	initializes the path for left turns.
	*/
	int BerniwPathfinding::initLeft(int id, double w)
	{
		int start = findStartSegId(id), end = findEndSegId(id);
		int prev = (start - 1 + ps.Count()) % ps.Count(), next = (end + 1) % ps.Count();
		int len = track->diffSegId(start, end);
		int tseg = (start + (len) / 2) % ps.Count();
		v3d* s1 = track->getSegmentPtr(start)->getRightBorder();
		v3d* s2 = track->getSegmentPtr(prev)->getRightBorder();
		v3d* tr = track->getSegmentPtr(prev)->getToRight();
		v3d* tg = track->getSegmentPtr(tseg)->getLeftBorder();
		v3d* trtg = track->getSegmentPtr(tseg)->getToRight();
		v3d sdir, sp, t;

		double beta = acos(track->cosalpha(trtg, tr));

		if (beta < 0.0) printf("error in initLeft: turn > 360° ??\n");

		s1->dirVector(s2, &sdir);
		sp = (*s2) - (*tr)*w;
		t = (*tg) + (*trtg)*w;

		double yd = track->distGFromPoint(&sp, &sdir, &t);
		int tlen = (int)ceil(clothlength(beta, yd));

		if (tlen < 0) printf("error in initLeft: tlen < 0 ??\n");

		int startsp = (tseg - tlen / 2 + ps.Count()) % ps.Count();
		int endsp = (startsp + tlen) % ps.Count();

		double s[3], y[3], ys[3];

		ys[0] = ys[1] = ys[2] = 0.0;
		s[0] = 0;
		s[1] = tlen / 2;
		s[2] = tlen;
		y[0] = track->getSegmentPtr(startsp)->getWidth() / 2.0 - w;
		y[1] = -(track->getSegmentPtr(tseg)->getWidth() / 2.0 - w);
		y[2] = track->getSegmentPtr(endsp)->getWidth() / 2.0 - w;

		double l = 0.0;
		v3d q, *pp, *qq;
		for (int i = startsp; (i + ps.Count()) % ps.Count() != endsp; i++) {
			int j = (i + ps.Count()) % ps.Count();
			double d = spline(3, l, s, y, ys);

			pp = track->getSegmentPtr(j)->getMiddle();
			qq = track->getSegmentPtr(j)->getToRight();
			q = (*pp) + (*qq)*d;
			setLocWeighted(j, computeWeight(((double)l) / ((double)tlen), tlen), &q);

			l += TRACKRES;
		}

		return next;
	}


	/*
	initializes the path for right turns.
	*/
	int BerniwPathfinding::initRight(int id, double w)
	{
		int start = findStartSegId(id), end = findEndSegId(id);
		int prev = (start - 1 + ps.Count()) % ps.Count(), next = (end + 1) % ps.Count();
		int len = track->diffSegId(start, end);
		int tseg = (start + (len) / 2) % ps.Count();
		v3d* s1 = track->getSegmentPtr(start)->getLeftBorder();
		v3d* s2 = track->getSegmentPtr(prev)->getLeftBorder();
		v3d* tr = track->getSegmentPtr(prev)->getToRight();
		v3d* tg = track->getSegmentPtr(tseg)->getRightBorder();
		v3d* trtg = track->getSegmentPtr(tseg)->getToRight();
		v3d sdir, sp, t;

		double beta = acos(track->cosalpha(trtg, tr));

		if (beta < 0.0) printf("error in initRight: turn > 360° ??\n");

		s1->dirVector(s2, &sdir);
		sp = (*s2) + (*tr)*w;
		t = (*tg) - (*trtg)*w;

		double yd = track->distGFromPoint(&sp, &sdir, &t);
		int tlen = (int)ceil(clothlength(beta, yd));

		if (tlen < 0) printf("error in initRight: tlen < 0 ??\n");

		int startsp = (tseg - tlen / 2 + ps.Count()) % ps.Count();
		int endsp = (startsp + tlen) % ps.Count();

		double s[3], y[3], ys[3];

		ys[0] = ys[1] = ys[2] = 0.0;
		s[0] = 0;
		s[1] = tlen / 2;
		s[2] = tlen;
		y[0] = -(track->getSegmentPtr(startsp)->getWidth() / 2.0 - w);
		y[1] = track->getSegmentPtr(tseg)->getWidth() / 2.0 - w;
		y[2] = -(track->getSegmentPtr(endsp)->getWidth() / 2.0 - w);

		double l = 0.0;
		v3d q, *pp, *qq;
		for (int i = startsp; (i + ps.Count()) % ps.Count() != endsp; i++) {
			int j = (i + ps.Count()) % ps.Count();
			double d = spline(3, l, s, y, ys);

			pp = track->getSegmentPtr(j)->getMiddle();
			qq = track->getSegmentPtr(j)->getToRight();
			q = (*pp) + (*qq)*d;
			setLocWeighted(j, computeWeight(((double)l) / ((double)tlen), tlen), &q);

			l += TRACKRES;
		}

		return next;
	}

	void BerniwPathfinding::smooth(int id, double delta, double w)
	{
		int ids[5] = { id - 2, id - 1, id, id + 1, id + 2 };
		double x[5], y[5], r, rmin = RMAX;
		PTrackSegment* t = track->getSegmentPtr(id);
		v3d* tr = t->getToRight();
		int i;

		for (i = 0; i < 5; i++) {
			ids[i] = (ids[i] + ps.Count()) % ps.Count();
			x[i] = ps(ids[i])->getLoc()->x;
			y[i] = ps(ids[i])->getLoc()->y;
		}

		for (i = 0; i < 3; i++) {
			r = fabs(radius(x[i], y[i], x[i + 1], y[i + 1], x[i + 2], y[i + 2]));
			if (r < rmin) rmin = r;
		}

		/* no optimisation needed */
		if (rmin == RMAX) return;

		double xp, yp, xm, ym, xo = x[2], yo = y[2], rp = RMAX, rm = RMAX;

		xp = x[2] = xo + delta*tr->x; yp = y[2] = yo + delta*tr->y;
		for (i = 0; i < 3; i++) {
			r = fabs(radius(x[i], y[i], x[i + 1], y[i + 1], x[i + 2], y[i + 2]));
			if (r < rp) rp = r;
		}

		xm = x[2] = xo - delta*tr->x; ym = y[2] = yo - delta*tr->y;
		for (i = 0; i < 3; i++) {
			r = fabs(radius(x[i], y[i], x[i + 1], y[i + 1], x[i + 2], y[i + 2]));
			if (r < rm) rm = r;
		}

		if (rp > rmin && rp > rm) {
			v3d n;
			n.x = xp;
			n.y = yp;
			n.z = ps(id)->getLoc()->z + delta*tr->z;
			ps(id)->setLoc(&n);
		}
		else if (rm > rmin && rm > rp) {
			v3d n;
			n.x = xm;
			n.y = ym;
			n.z = ps(id)->getLoc()->z - delta*tr->z;
			ps(id)->setLoc(&n);
		}
	}

	void BerniwPathfinding::smooth(int s, int p, int e, double w)
	{
		PTrackSegment* t = track->getSegmentPtr(p);
		v3d *rgh = t->getToRight();
		v3d *rs = ps(s)->getLoc(), *rp = ps(p)->getLoc(), *re = ps(e)->getLoc(), n;

		double rgx = (re->x - rs->x), rgy = (re->y - rs->y);
		double m = (rs->x * rgy + rgx * rp->y - rs->y * rgx - rp->x * rgy) / (rgy * rgh->x - rgx * rgh->y);

		n = (*rp) + (*rgh)*m;

		ps(p)->setLoc(&n);
	}


	void BerniwPathfinding::optimize(int start, int range, double w)
	{
		for (int p = start; p < start + range; p = p + 1) {
			int j = (p) % ps.Count();
			int k = (p + 1) % ps.Count();
			int l = (p + 2) % ps.Count();
			smooth(j, k, l, w);
		}
	}


	void BerniwPathfinding::optimize2(int start, int range, double w)
	{
		for (int p = start; p < start + range; p = p + 1) {
			int j = (p) % ps.Count();
			int k = (p + 1) % ps.Count();
			int l = (p + 2) % ps.Count();
			int m = (p + 3) % ps.Count();
			smooth(j, k, m, w);
			smooth(j, l, m, w);
		}
	}


	void BerniwPathfinding::optimize3(int start, int range, double w)
	{
		for (int p = start; p < start + range; p = p + 3) {
			int j = (p) % ps.Count();
			int k = (p + 1) % ps.Count();
			int l = (p + 2) % ps.Count();
			int m = (p + 3) % ps.Count();
			smooth(j, k, m, w);
			smooth(j, l, m, w);
		}
	}
}