#include "PPathPlanner.h"

namespace procPathfinder
{
	const double PPathPlanner::COLLDIST = 200.0;

	PPathPlanner::PPathPlanner(PPathfinder* pathfinder, tSituation* s)
	{
		this->path = pathfinder;

		changed = lastPlan = lastPlanRange = 0;

		Init(s);
	}

	PPathPlanner::~PPathPlanner()
	{
		if (path)
			delete path;
		if (o)
			delete o;
	}

	void PPathPlanner::Init(tSituation* s)
	{
		int i;
		o = new tOCar[s->_ncars];
		overlaptimer = new tOverlapTimer[s->_ncars];

		for (i = 0; i < s->_ncars; i++) overlaptimer[i].time = 0.0;
	}

	void PPathPlanner::Plan(int trackSegId, tCarElt* car, tSituation* situation, PCarDesc* myc, POtherCarDesc* ocar)
	{
		double r, length, speedsqr;
		int u, v, w;
		v3d dir;

		int i, start;

		if (myc->derror > myc->PATHERR*myc->PATHERRFACTOR) {
			start = trackSegId;
		}
		else {
			start = lastPlan + lastPlanRange;
		}
		
		/* load precomputed trajectory */
		for (i = start; i < trackSegId + path->LookAhead() + SEGRANGE; i++) {
			int j = (i + path->getnPathSeg()) % path->getnPathSeg();
			path->Seg(j)->setLoc(path->Seg(j)->getOptLoc());
		}


		collcars = UpdateOCar(trackSegId, situation, myc, ocar, o);
		UpdateOverlapTimer(trackSegId, situation, myc, ocar, o, overlaptimer);

		/* are we on the trajectory or do i need a correction */
		if ((myc->derror > myc->PATHERR*myc->PATHERRFACTOR ||
			(myc->getDeltaPitch() > myc->MAXALLOWEDPITCH && myc->getSpeed() > myc->FLYSPEED))) {
			changed += CorrectPath(trackSegId, car, myc);
		}

		/* overtaking */
		if (changed == 0) {
			changed += Overtake(trackSegId, situation, myc, ocar);
		}

		/* if we have nothing better to, let opponents overlap */
		if (changed == 0) {
			changed = Letoverlap(trackSegId, situation, myc, ocar, overlaptimer);
		}

		/* recompute speed and direction of new trajectory */
		if (changed > 0 || (path->Seg(trackSegId)->getSpeedsqr() < 5.0)) {
			start = trackSegId;
		}

		u = start - 1; v = start; w = start + 1;
		int u2 = (start - 3 + path->getnPathSeg()) % path->getnPathSeg();
		int w2 = (start + 3 + path->getnPathSeg()) % path->getnPathSeg();
		u = (u + path->getnPathSeg()) % path->getnPathSeg();
		v = (v + path->getnPathSeg()) % path->getnPathSeg();
		w = (w + path->getnPathSeg()) % path->getnPathSeg();

		for (i = start; i < trackSegId + path->LookAhead() + SEGRANGE; i++) {
			if (i > path->getnPathSeg()) break;
			int j = (i + path->getnPathSeg()) % path->getnPathSeg();
			/* taking 2 radiuses to reduce "noise" */
			double r2 = radius(path->Seg(u)->getLoc()->x, path->Seg(u)->getLoc()->y,
				path->Seg(v)->getLoc()->x, path->Seg(v)->getLoc()->y, path->Seg(w)->getLoc()->x, path->Seg(w)->getLoc()->y);
			double r1 = radius(path->Seg(u2)->getLoc()->x, path->Seg(u2)->getLoc()->y,
				path->Seg(v)->getLoc()->x, path->Seg(v)->getLoc()->y, path->Seg(w2)->getLoc()->x, path->Seg(w2)->getLoc()->y);

			if (fabs(r1) > fabs(r2)) {
				path->Seg(j)->setRadius(r1);
				r = fabs(r1);
			}
			else {
				path->Seg(j)->setRadius(r2);
				r = fabs(r2);
			}

			length = path->dist(path->Seg(v)->getLoc(), path->Seg(w)->getLoc());

			/* compute allowed speedsqr */
			double mu = path->Track()->getSegmentPtr(j)->getKfriction()*myc->CFRICTION*path->Track()->getSegmentPtr(j)->getKalpha();
			double b = path->Track()->getSegmentPtr(j)->getKbeta();
			speedsqr = myc->SPEEDSQRFACTOR*r*g*mu / (1.0 - MIN(1.0, (mu*myc->ca*r / myc->mass)) + mu*r*b);

			dir = (*path->Seg(w)->getLoc()) - (*path->Seg(u)->getLoc());
			dir.normalize();

			//path->Seg(j)->set(speedsqr, length, path->Seg(v)->getLoc(), &dir);
			path->Seg(j)->set(speedsqr, length, &dir);

			u = v; v = w; w = (w + 1 + path->getnPathSeg()) % path->getnPathSeg();
			w2 = (w2 + 1 + path->getnPathSeg()) % path->getnPathSeg();
			u2 = (u2 + 1 + path->getnPathSeg()) % path->getnPathSeg();
		}

		changed = 0;

		/* set speed limits on the path, in case there is an obstacle (other car) */
		changed += Collision(trackSegId, car, situation, myc, ocar);

		lastPlan = trackSegId; lastPlanRange = path->LookAhead();
	}

	/* collision avoidence with braking */
	int PPathPlanner::Collision(int trackSegId, tCarElt* mycar, tSituation* s, PCarDesc* myc, POtherCarDesc* ocar)
	{
		int end = (trackSegId + (int)COLLDIST + path->getnPathSeg()) % path->getnPathSeg();
		int didsomething = 0;
		int i, n = collcars;

		for (i = 0; i < n; i++) {
			if (o[i].overtakee == true) continue;
			int currentsegid = o[i].collcar->getCurrentSegId();
			if (path->Track()->isBetween(trackSegId, end, currentsegid) && (myc->getSpeed() > o[i].speed)) {
				int spsegid = (currentsegid - (int)(myc->CARLEN + 1) + path->getnPathSeg()) % path->getnPathSeg();

				if (o[i].mincorner < myc->CARWIDTH / 2.0 + myc->DIST) {
					double cmpdist = o[i].dist - myc->CARLEN - myc->DIST;
					if ((o[i].brakedist >= cmpdist) && (path->Seg(spsegid)->getSpeedsqr() > o[i].speedsqr)) {
						int j;
						for (j = spsegid - 3; j < spsegid + 3; j++) {
							path->Seg((j + path->getnPathSeg()) % path->getnPathSeg())->setSpeedsqr(o[i].speedsqr);
						}
						didsomething = 1;
					}
				}

				if (path->Track()->isBetween(trackSegId, end, o[i].catchsegid)) {
					double myd = path->Track()->distToMiddle(o[i].catchsegid, path->Seg(o[i].catchsegid)->getLoc());
					v3d r;
					o[i].collcar->getDir()->crossProduct(myc->getDir(), &r);
					double sina = r.len()*sign(r.z);
					double otherd = o[i].disttomiddle + sina*o[i].collcar->getSpeed()*o[i].time;

					if (fabs(myd - otherd) < myc->CARWIDTH + myc->DIST) {
						if ((o[i].catchdist > 0.0) && (o[i].brakedist >= (o[i].catchdist - (myc->CARLEN + myc->DIST)))) {
							PathSeg* catchseg = path->Seg((o[i].catchsegid - (int)myc->CARLEN + path->getnPathSeg()) % path->getnPathSeg());
							if (catchseg->getSpeedsqr() > o[i].speedsqr) {
								catchseg->setSpeedsqr(o[i].speedsqr);
								didsomething = 1;
							}
						}
					}
				}
			}
		}
		return didsomething;
	}

	/* compute a path back to the planned path */
	int PPathPlanner::CorrectPath(int id, tCarElt* car, PCarDesc* myc)
	{
		double s[2], y[2], ys[2];
		bool out;

		double d = path->Track()->distToMiddle(id, myc->getCurrentPos());
		//	double factor = MIN(myc->CORRLEN*fabs(d), path->getnPathSeg()()/2.0);
		double factor = MIN(MIN(myc->CORRLEN*myc->derror, path->getnPathSeg() / 2.0), path->LookAhead());
		int endid = (id + (int)(factor)+path->getnPathSeg()) % path->getnPathSeg();

		if (fabs(d) > (path->Track()->getSegmentPtr(id)->getWidth() - myc->CARWIDTH) / 2.0) {
			d = sign(d)*((path->Track()->getSegmentPtr(id)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN);
			ys[0] = 0.0;
			out = true;
		}
		else {
			v3d pathdir = *path->Seg(id)->getDir();
			pathdir.z = 0.0;
			pathdir.normalize();
			double alpha = PI / 2.0 - acos((*myc->getDir())*(*path->Track()->getSegmentPtr(id)->getToRight()));
			ys[0] = tan(alpha);
			out = false;
		}

		double ed = path->Track()->distToMiddle(endid, path->Seg(endid)->getLoc());

		/* set up points */
		y[0] = d;

		y[1] = ed;
		ys[1] = PathSlope(endid);

		s[0] = 0.0;
		s[1] = CountSegments(id, endid);

		/* modify path */
		double l = 0.0;
		v3d q, *pp, *qq;
		int i, j;

		if (out == true) {
			for (i = id; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != endid; i++) {
				d = spline(2, l, s, y, ys);
				if (fabs(d) > (path->Track()->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0) {
					d = sign(d)*((path->Track()->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN);
				}

				pp = path->Track()->getSegmentPtr(j)->getMiddle();
				qq = path->Track()->getSegmentPtr(j)->getToRight();
				q = (*pp) + (*qq)*d;
				path->Seg(j)->setLoc(&q);
				l += TRACKRES;
			}
		}
		else {
			double* newdisttomiddle = new double[path->LookAhead()];
			for (i = id; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != endid; i++) {
				d = spline(2, l, s, y, ys);
				if (fabs(d) > (path->Track()->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN) {
					return 0;
				}
				newdisttomiddle[i - id] = d;
				l += TRACKRES;
			}

			for (i = id; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != endid; i++) {
				pp = path->Track()->getSegmentPtr(j)->getMiddle();
				qq = path->Track()->getSegmentPtr(j)->getToRight();
				q = *pp + (*qq)*newdisttomiddle[i - id];
				path->Seg(j)->setLoc(&q);
			}

			delete[] newdisttomiddle;
		}

		/* align previos point for getting correct speedsqr in PPathPlanner::plan(...) */
		int p = (id - 1 + path->getnPathSeg()) % path->getnPathSeg();
		int e = (id + 1 + path->getnPathSeg()) % path->getnPathSeg();
		Smooth(id, p, e, 1.0);

		return 1;
	}

	/* compute path for overtaking the "next colliding" car */
	int PPathPlanner::Overtake(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar)
	{
		if (collcars == 0) return 0;

		const int start = (trackSegId - (int)(2.0 + myc->CARLEN) + path->getnPathSeg()) % path->getnPathSeg();
		const int nearend = (trackSegId + (int)(2.0*myc->CARLEN)) % path->getnPathSeg();

		POtherCarDesc* nearestCar = NULL;	/* car near in time, not in space ! (next reached car) */
		double minTime = FLT_MAX;
		double minorthdist = FLT_MAX;	/* near in space */
		double orthdist = FLT_MAX;
		int minorthdistindex = 0;
		int collcarindex = 0;

		int i, m = 0;
		for (i = 0; i < collcars; i++) {
			if (o[i].dist < COLLDIST / 3) {
				double dst = o[i].minorthdist;
				if (o[i].time > 0.0 && o[i].time < minTime) {
					minTime = o[i].time;
					collcarindex = i;
					orthdist = dst;
				}
				if (dst < minorthdist && path->Track()->isBetween(start, nearend, o[i].collcar->getCurrentSegId())) {
					minorthdist = dst;
					minorthdistindex = i;
				}
				m++;
			}
		}

		if (m == 0) return 0;

		bool sidechangeallowed;

		if (minorthdist <= myc->OVERTAKEMINDIST && o[collcarindex].dist >= o[minorthdistindex].dist) {
			collcarindex = minorthdistindex;
			nearestCar = o[minorthdistindex].collcar;
			sidechangeallowed = false;
		}
		else if (minTime < FLT_MAX){
			nearestCar = o[collcarindex].collcar;
			sidechangeallowed = true;
			minorthdist = orthdist;
			int i;
			for (i = 0; i <= (int)myc->MINOVERTAKERANGE; i += 10) {
				if (path->Track()->getSegmentPtr((trackSegId + i) % path->getnPathSeg())->getRadius() < myc->OVERTAKERADIUS) return 0;
			}
		}
		else return 0;

		/* not enough space, so we try to overtake */
		if (((o[collcarindex].mincorner < myc->CARWIDTH / 2.0 + myc->DIST) && (minTime < myc->TIMETOCATCH)) || !sidechangeallowed) {
			int overtakerange = (int)MIN(MAX((3 * minTime*myc->getSpeed()), myc->MINOVERTAKERANGE), path->LookAhead() - 50);
			double d = o[collcarindex].disttomiddle;
			double mydisttomiddle = path->Track()->distToMiddle(myc->getCurrentSegId(), myc->getCurrentPos());
			double y[3], ys[3], s[3];

			y[0] = path->Track()->distToMiddle(trackSegId, myc->getCurrentPos());
			double alpha = PI / 2.0 - acos((*myc->getDir())*(*path->Track()->getSegmentPtr(trackSegId)->getToRight()));
			int trackSegId1 = (trackSegId + overtakerange / 3) % path->getnPathSeg();
			double w = path->Track()->getSegmentPtr(nearestCar->getCurrentSegId())->getWidth() / 2;
			double pathtomiddle = path->Track()->distToMiddle(trackSegId1, path->Seg(trackSegId1)->getLoc());
			double paralleldist = o[collcarindex].cosalpha*path->dist(myc->getCurrentPos(), nearestCar->getCurrentPos());

			if (!sidechangeallowed) {
				if (paralleldist > 1.5*myc->CARLEN) {
					int i;
					for (i = 0; i <= (int)myc->MINOVERTAKERANGE; i += 10) {
						if (path->Track()->getSegmentPtr((trackSegId + i) % path->getnPathSeg())->getRadius() < myc->OVERTAKERADIUS) return 0;
					}
					v3d r, dir = *o[collcarindex].collcar->getCurrentPos() - *myc->getCurrentPos();
					myc->getDir()->crossProduct(&dir, &r);
					double pathtocarsgn = sign(r.z);

					y[1] = d + myc->OVERTAKEDIST*pathtocarsgn;
					if (fabs(y[1]) > w - (1.5*myc->CARWIDTH)) {
						y[1] = d - myc->OVERTAKEDIST*pathtocarsgn;
					}
					double beta = PI / 2.0 - acos((*nearestCar->getDir())*(*path->Track()->getSegmentPtr(trackSegId)->getToRight()));
					if (y[1] - mydisttomiddle >= 0.0) {
						if (alpha < beta + myc->OVERTAKEANGLE) alpha = alpha + myc->OVERTAKEANGLE;
					}
					else {
						if (alpha > beta - myc->OVERTAKEANGLE) alpha = alpha - myc->OVERTAKEANGLE;
					}
				}
				else {
					double beta = PI / 2.0 - acos((*nearestCar->getDir())*(*path->Track()->getSegmentPtr(trackSegId)->getToRight()));
					double delta = mydisttomiddle - d;
					if (delta >= 0.0) {
						if (alpha < beta + myc->OVERTAKEANGLE) alpha = beta + myc->OVERTAKEANGLE;
					}
					else {
						if (alpha > beta - myc->OVERTAKEANGLE) alpha = beta - myc->OVERTAKEANGLE;
					}
					double cartocarsgn = sign(delta);
					y[1] = d + myc->OVERTAKEDIST*cartocarsgn;
					if (fabs(y[1]) > w - (1.5*myc->CARWIDTH)) {
						y[1] = cartocarsgn*(w - (myc->CARWIDTH + myc->MARGIN));
					}
					if (minorthdist > myc->OVERTAKEMINDIST) o[collcarindex].overtakee = true;
				}
			}
			else {
				double pathtocarsgn = sign(pathtomiddle - d);
				y[1] = d + myc->OVERTAKEDIST*pathtocarsgn;
				if (fabs(y[1]) > w - (1.5*myc->CARWIDTH)) {
					y[1] = d - myc->OVERTAKEDIST*pathtocarsgn;
				}
			}

			double ww = w - (myc->CARWIDTH + myc->MARGIN);
			if ((y[1] > ww && alpha > 0.0) || (y[1] < -ww && alpha < 0.0)) {
				alpha = 0.0;
			}

			//ys[0] = sin(alpha);
			ys[0] = tan(alpha);
			ys[1] = 0.0;

			/* set up point 2 */
			int trackSegId2 = (trackSegId + overtakerange) % path->getnPathSeg();
			y[2] = path->Track()->distToMiddle(trackSegId2, path->Seg(trackSegId2)->getOptLoc());
			ys[2] = PathSlope(trackSegId2);

			/* set up parameter s */
			s[0] = 0.0;
			s[1] = CountSegments(trackSegId, trackSegId1);
			s[2] = s[1] + CountSegments(trackSegId1, trackSegId2);

			/* check path for leaving to path->Track() */
			double* newdisttomiddle = new double[path->LookAhead()];
			int i, j;
			double l = 0.0; v3d q, *pp, *qq;
			for (i = trackSegId; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != trackSegId2; i++) {
				d = spline(3, l, s, y, ys);
				if (fabs(d) > (path->Track()->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN) {
					o[collcarindex].overtakee = false;
					return 0;
				}
				newdisttomiddle[i - trackSegId] = d;
				l += TRACKRES;
			}

			/* set up the path */
			for (i = trackSegId; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != trackSegId2; i++) {
				pp = path->Track()->getSegmentPtr(j)->getMiddle();
				qq = path->Track()->getSegmentPtr(j)->getToRight();
				q = *pp + (*qq)*newdisttomiddle[i - trackSegId];
				path->Seg(j)->setLoc(&q);
			}

			delete[] newdisttomiddle;

			/* reload old trajectory where needed */
			for (i = trackSegId2; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != (trackSegId + path->LookAhead()) % path->getnPathSeg(); i++) {
				path->Seg(j)->setLoc(path->Seg(j)->getOptLoc());
			}

			/* align previos point for getting correct speedsqr in PPathPlanner::plan(...) */
			int p = (trackSegId - 1 + path->getnPathSeg()) % path->getnPathSeg();
			int e = (trackSegId + 1 + path->getnPathSeg()) % path->getnPathSeg();
			Smooth(trackSegId, p, e, 1.0);

			return 1;
		}
		else {
			return 0;
		}
	}


	/* collect data about other cars relative to me */
	inline int PPathPlanner::UpdateOCar(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o)
	{
		const int start = (trackSegId - (int)(1.0 + myc->CARLEN / 2.0) + path->getnPathSeg()) % path->getnPathSeg();
		const int end = (trackSegId + (int)COLLDIST + path->getnPathSeg()) % path->getnPathSeg();

		int i, n = 0;		/* counter for relevant cars */

		for (i = 0; i < s->_ncars; i++) {
			tCarElt* car = ocar[i].getCarPtr();
			/* is it me ? */
			if (car != myc->getCarPtr()) {
				int seg = ocar[i].getCurrentSegId();
				/* get the next car to catch up */
				if (path->Track()->isBetween(start, end, seg) && !(car->_state & (RM_CAR_STATE_DNF | RM_CAR_STATE_PULLUP | RM_CAR_STATE_PULLSIDE | RM_CAR_STATE_PULLDN))) {
					o[n].cosalpha = (*myc->getDir())*(*ocar[i].getDir());
					o[n].speed = ocar[i].getSpeed()*o[n].cosalpha;
					int j, k = path->Track()->diffSegId(trackSegId, seg);
					if (k < 40) {
						o[n].dist = 0.0;
						int l = MIN(trackSegId, seg);
						for (j = l; j < l + k; j++) o[n].dist += path->Seg(j % path->getnPathSeg())->getLength();
						if (o[n].dist > k) o[n].dist = k;
					}
					else {
						o[n].dist = k;
					}
					o[n].collcar = &ocar[i];
					o[n].time = o[n].dist / (myc->getSpeed() - o[n].speed);
					o[n].disttomiddle = path->Track()->distToMiddle(seg, ocar[i].getCurrentPos());
					o[n].speedsqr = sqr(o[n].speed);
					o[n].catchdist = (int)(o[n].dist / (myc->getSpeed() - ocar[i].getSpeed())*myc->getSpeed());
					o[n].catchsegid = (o[n].catchdist + trackSegId + path->getnPathSeg()) % path->getnPathSeg();
					o[n].overtakee = false;
					o[n].disttopath = path->distToPath(seg, ocar[i].getCurrentPos());
					double gm = path->Track()->getSegmentPtr(seg)->getKfriction()*myc->CFRICTION;
					double qs = o[n].speedsqr;
					o[n].brakedist = (myc->getSpeedSqr() - o[n].speedsqr)*(myc->mass / (2.0*gm*g*myc->mass + (qs)*(gm*myc->ca)));
					o[n].mincorner = FLT_MAX;
					o[n].minorthdist = FLT_MAX;
					for (j = 0; j < 4; j++) {
						v3d e(car->pub.corner[j].ax, car->pub.corner[j].ay, car->_pos_Z);
						double corner = fabs(path->distToPath(seg, &e));
						double orthdist = path->Track()->distGFromPoint(myc->getCurrentPos(), myc->getDir(), &e) - myc->CARWIDTH / 2.0;
						if (corner < o[n].mincorner) o[n].mincorner = corner;
						if (orthdist < o[n].minorthdist) o[n].minorthdist = orthdist;
					}
					n++;
				}
			}
		}
		return n;
	}


	inline void PPathPlanner::UpdateOverlapTimer(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o, tOverlapTimer* ov)
	{
		const int start = (trackSegId - (int)myc->OVERLAPSTARTDIST + path->getnPathSeg()) % path->getnPathSeg();
		const int end = (trackSegId - (int)(2.0 + myc->CARLEN / 2.0) + path->getnPathSeg()) % path->getnPathSeg();
		const int startfront = (trackSegId + (int)(2.0 + myc->CARLEN / 2.0)) % path->getnPathSeg();
		const int endfront = (trackSegId + (int)myc->OVERLAPSTARTDIST) % path->getnPathSeg();

		int i;

		for (i = 0; i < s->_ncars; i++) {
			tCarElt* car = ocar[i].getCarPtr();
			tCarElt* me = myc->getCarPtr();
			/* is it me, and in case not, has the opponent more laps than me? */
			if ((car != me) && (car->race.laps > me->race.laps) &&
				!(car->_state & (RM_CAR_STATE_DNF | RM_CAR_STATE_PULLUP | RM_CAR_STATE_PULLSIDE | RM_CAR_STATE_PULLDN))) {
				int seg = ocar[i].getCurrentSegId();
				if (path->Track()->isBetween(start, end, seg)) {
					ov[i].time += s->deltaTime;
				}
				else if (path->Track()->isBetween(startfront, endfront, seg)) {
					ov[i].time = myc->LAPBACKTIMEPENALTY;
				}
				else {
					if (ov[i].time > 0.0) ov[i].time -= s->deltaTime;
					else ov[i].time += s->deltaTime;
				}
			}
			else {
				ov[i].time = 0.0;
			}
		}
	}


	/* compute trajectory to let opponent overlap */
	int PPathPlanner::Letoverlap(int trackSegId, tSituation *situation, PCarDesc* myc, POtherCarDesc* ocar, tOverlapTimer* ov)
	{
		const int start = (trackSegId - (int)myc->OVERLAPPASSDIST + path->getnPathSeg()) % path->getnPathSeg();
		const int end = (trackSegId - (int)(2.0 + myc->CARLEN / 2.0) + path->getnPathSeg()) % path->getnPathSeg();
		int k;

		for (k = 0; k < situation->_ncars; k++) {
			if ((ov[k].time > myc->OVERLAPWAITTIME) && path->Track()->isBetween(start, end, ocar[k].getCurrentSegId())) {
				/* let overtake */
				double s[4], y[4], ys[4];
				const int DST = 400;

				ys[0] = PathSlope(trackSegId);
				if (fabs(ys[0]) > PI / 180.0) return 0;

				int trackSegId1 = (trackSegId + (int)DST / 4 + path->getnPathSeg()) % path->getnPathSeg();
				int trackSegId2 = (trackSegId + (int)DST * 3 / 4 + path->getnPathSeg()) % path->getnPathSeg();
				int trackSegId3 = (trackSegId + (int)DST + path->getnPathSeg()) % path->getnPathSeg();
				double width = path->Track()->getSegmentPtr(trackSegId1)->getWidth();

				/* point 0 */
				y[0] = path->Track()->distToMiddle(trackSegId, myc->getCurrentPos());

				/* point 1 */
				y[1] = sign(y[0])*MIN((width / 2.0 - 2.0*myc->CARWIDTH - myc->MARGIN), (15.0 / 2.0));
				ys[1] = 0.0;

				/* point 2 */
				y[2] = y[1];
				ys[2] = 0.0;

				/* point 3*/
				y[3] = path->Track()->distToMiddle(trackSegId3, path->Seg(trackSegId3)->getOptLoc());
				ys[3] = PathSlope(trackSegId3);

				/* set up parameter s */
				s[0] = 0.0;
				s[1] = CountSegments(trackSegId, trackSegId1);
				s[2] = s[1] + CountSegments(trackSegId1, trackSegId2);
				s[3] = s[2] + CountSegments(trackSegId2, trackSegId3);

				/* check path for leaving to path->Track() */
				double* newdisttomiddle = new double[path->LookAhead()];
				double d;
				int i, j;
				double l = 0.0; v3d q, *pp, *qq;
				for (i = trackSegId; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != trackSegId3; i++) {
					d = spline(4, l, s, y, ys);
					if (fabs(d) > (path->Track()->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN) {
						return 0;
					}
					newdisttomiddle[i - trackSegId] = d;
					l += TRACKRES;
				}

				/* set up the path */
				for (i = trackSegId; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != trackSegId3; i++) {
					pp = path->Track()->getSegmentPtr(j)->getMiddle();
					qq = path->Track()->getSegmentPtr(j)->getToRight();
					q = *pp + (*qq)*newdisttomiddle[i - trackSegId];
					path->Seg(j)->setLoc(&q);
				}

				delete[] newdisttomiddle;

				/* reload old trajectory where needed */
				for (i = trackSegId3; (j = (i + path->getnPathSeg()) % path->getnPathSeg()) != (trackSegId + path->LookAhead()) % path->getnPathSeg(); i++) {
					path->Seg(j)->setLoc(path->Seg(j)->getOptLoc());
				}

				/* reset all timer to max 3.0 */
				for (j = 0; j < situation->_ncars; j++) {
					ov[j].time = MIN(ov[j].time, 3.0);
				}
				return 1;
			}
		}
		return 0;
	}

	void PPathPlanner::Smooth(int s, int p, int e, double w)
	{
		PTrackSegment* t = path->Track()->getSegmentPtr(p);
		v3d *rgh = t->getToRight();
		v3d *rs = path->Seg(s)->getLoc(), *rp = path->Seg(p)->getLoc(), *re = path->Seg(e)->getLoc(), n;

		double rgx = (re->x - rs->x), rgy = (re->y - rs->y);
		double m = (rs->x * rgy + rgx * rp->y - rs->y * rgx - rp->x * rgy) / (rgy * rgh->x - rgx * rgh->y);

		n = (*rp) + (*rgh)*m;

		path->Seg(p)->setLoc(&n);
	}

	double PPathPlanner::PathSlope(int id)
	{
		int nextid = (id + 1) % path->getnPathSeg();
		v3d dir = *path->Seg(nextid)->getLoc() - *path->Seg(id)->getLoc();
		double dp = dir*(*path->Track()->getSegmentPtr(id)->getToRight()) / dir.len();
		double alpha = PI / 2.0 - acos(dp);
		return tan(alpha);
	}

	int PPathPlanner::CountSegments(int from, int to)
	{
		if (to >= from) {
			return to - from;
		}
		else {
			return path->getnPathSeg() - from + to;
		}
	}
}