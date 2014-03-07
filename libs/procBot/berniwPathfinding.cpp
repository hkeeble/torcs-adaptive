/*
	File: PPPathfinder.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Pathfinding functions from the berniw robot.
*/

#include "PPathfinder.h"
#include "spline.h"

namespace procBot
{
	/*
	computes int(sin(u^2), u=0..alpha), where alpha is [0..PI).
	*/
	double PPathfinder::intsinsqr(double alpha)
	{
		int i = (int)floor(alpha / TPRES), j = i + 1;
		/* linear interpoation between the nearest known two points */
		return cp[i].is + (alpha - cp[i].x)*(cp[j].is - cp[i].is) / TPRES;
	}


	/*
	computes int(cos(u^2), u=0..alpha), where alpha is [0..PI).
	*/
	double PPathfinder::intcossqr(double alpha)
	{
		int i = (int)floor(alpha / TPRES), j = i + 1;
		/* linear interpoation between the nearest known two points */
		return cp[i].ic + (alpha - cp[i].x)*(cp[j].ic - cp[i].ic) / TPRES;
	}

	/*
	searches for the startid of a part, eg. TR_STR
	*/

	int PPathfinder::findStartSegId(int id)
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
	int PPathfinder::findEndSegId(int id)
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
	double PPathfinder::computeWeight(double x, double len)
	{
		return (x <= 0.5) ? (2.0*x)*len : (2.0*(1.0 - x))*len;
	}


	/*
	modify point according to the weights
	*/
	void PPathfinder::setLocWeighted(int id, double newweight, v3d* newp)
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
	int PPathfinder::initStraight(int id, double w)
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
	int PPathfinder::initLeft(int id, double w)
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
	int PPathfinder::initRight(int id, double w)
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
}