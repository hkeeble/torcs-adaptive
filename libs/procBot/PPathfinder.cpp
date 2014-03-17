#include "PPathfinder.h"

namespace procBot
{
	const double PPathfinder::COLLDIST = 200.0;
	const double PPathfinder::TPRES = PI / (NTPARAMS - 1);	/* resolution of the steps */

	PPathfinder::PPathfinder(PTrackDesc* itrack, PCarDesc* carDesc, tSituation *s)
	{
		track = itrack;
		car = carDesc->getCarPtr();;
		this->carDesc = carDesc;
		previousPSCount = 0;
		Init(s);

		// Initialize the state manager
		stateMngr = PStateManager(track->GetTorcsTrack());

		// Plan a static route using the car description, without taking into account the current situation
		staticPlan(carDesc);

		ahead = track->GetTorcsTrack()->length - getCurrentSegment(car);
	}

	void PPathfinder::Init(tSituation* s)
	{
		int i;
		tTrack* t = track->GetTorcsTrack();
		o = new tOCar[s->_ncars];
		overlaptimer = new tOverlapTimer[s->_ncars];

		for (i = 0; i < s->_ncars; i++) overlaptimer[i].time = 0.0;

		/* Initialize Path Segment Collection */
		ps = PathSegCollection(track->segmentCount());
		changed = lastPlan = lastPlanRange = 0;
		inPit = pitStop = false;

		/* check if there is a pit type we can use and if for this car is a pit available */
		pit = false;
		if (t->pits.type == TR_PIT_ON_TRACK_SIDE && car->index < t->pits.nMaxPits) {
			pit = true;
		}

		s1 = e3 = 0;
		if (isPitAvailable()) {
			initPit(car);
			s1 = track->getPitEntryStartId();
			s1 = (int)GfParmGetNum(car->_carHandle, "berniProc private", "pitentry", (char*)NULL, (float)s1);
			e3 = track->getPitExitEndId();
			e3 = (int)GfParmGetNum(car->_carHandle, "berniProc private", "pitexit", (char*)NULL, (float)e3);
			pitspeedsqrlimit = t->pits.speedLimit - 0.5;
			pitspeedsqrlimit *= pitspeedsqrlimit;
			/* get memory for the pit points */
			pitcord = new v3d[countSegments(s1, e3)];
		}
	}

	PPathfinder::~PPathfinder()
	{
		if (isPitAvailable()) delete[] pitcord;
		delete[] o;
		delete[] overlaptimer;
	}

	/* compute where the pit is, etc */
	void PPathfinder::initPit(tCarElt* car)
	{
		tTrack* t = track->GetTorcsTrack();

		if (t->pits.driversPits != NULL && car != NULL) {
			if (isPitAvailable()) {
				tTrackSeg* pitSeg = t->pits.driversPits->pos.seg;
				if (pitSeg->type == TR_STR) {
					v3d v1, v2;
					/* v1 points in the direction of the segment */
					v1.x = pitSeg->vertex[TR_EL].x - pitSeg->vertex[TR_SL].x;
					v1.y = pitSeg->vertex[TR_EL].y - pitSeg->vertex[TR_SL].y;
					v1.z = pitSeg->vertex[TR_EL].z - pitSeg->vertex[TR_SL].z;
					v1.normalize();

					/* v2 points to the side of the segment */
					double s = (t->pits.side == TR_LFT) ? -1.0 : 1.0;
					v2.x = s*(pitSeg->vertex[TR_SR].x - pitSeg->vertex[TR_SL].x);
					v2.y = s*(pitSeg->vertex[TR_SR].y - pitSeg->vertex[TR_SL].y);
					v2.z = s*(pitSeg->vertex[TR_SR].z - pitSeg->vertex[TR_SL].z);
					v2.normalize();

					/* loading starting point of segment */
					pitLoc.x = (pitSeg->vertex[TR_SR].x + pitSeg->vertex[TR_SL].x) / 2.0;
					pitLoc.y = (pitSeg->vertex[TR_SR].y + pitSeg->vertex[TR_SL].y) / 2.0;
					pitLoc.z = (pitSeg->vertex[TR_SR].z + pitSeg->vertex[TR_SL].z) / 2.0;

					/* going along the track */
					double l = t->pits.len*car->index;
					pitLoc = pitLoc + (t->pits.driversPits->pos.toStart + l)*v1;

					/* going sideways, minus because of opposite sign of v2 and the value toMiddle */
					double m = fabs(t->pits.driversPits->pos.toMiddle);
					pitLoc = pitLoc + m*v2;

					pitSegId = track->getNearestId(&pitLoc);

					l = t->pits.len*(car->index + 2);
					v2 = pitLoc - l*v1;
					s3 = track->getNearestId(&v2);

					l = t->pits.len*(t->pits.nMaxPits + 1 + 2);
					v2 = v2 + l*v1;
					e1 = track->getNearestId(&v2);
				}
				else pit = false;
			}
		}
		else
		{
			printf("error: pit struct ptr == NULL. call this NOT in inittrack, call it in newrace.\n");
		}
	}


	/* call this after you computed a static racing path with plan() */
	void PPathfinder::initPitStopPath(void)
	{
		tTrack* t = track->GetTorcsTrack();
		v3d p, q, *pp, *qq, *pmypitseg = track->getSegmentPtr(pitSegId)->getMiddle();
		double d, dp, sgn;
		double delta = t->pits.width;
		int i;
		double ypit[PITPOINTS], yspit[PITPOINTS], spit[PITPOINTS];
		int snpit[PITPOINTS];

		/* set up point 0 on the track (s1) */
		ypit[0] = track->distToMiddle(s1, ps(s1)->getLoc());
		snpit[0] = s1;

		/* set up point 1 pit lane entry (s3) */
		track->dirVector2D(&pitLoc, pmypitseg, &p);
		dp = p.len();
		d = dp - delta;

		sgn = (t->pits.side == TR_LFT) ? -1.0 : 1.0;
		ypit[1] = d*sgn;
		snpit[1] = s3;

		/* set up point 2 before we turn into the pit */
		i = (pitSegId - (int)t->pits.len + ps.Count()) % ps.Count();
		ypit[2] = d*sgn;
		snpit[2] = i;

		/* set up point 3, the pit, we know this already */
		ypit[3] = dp*sgn;
		snpit[3] = pitSegId;

		/* compute point 4, go from pit back to pit lane */
		i = (pitSegId + (int)t->pits.len + ps.Count()) % ps.Count();
		ypit[4] = d*sgn;
		snpit[4] = i;

		/* compute point 5, drive to end of pit lane (e1) */
		ypit[5] = d*sgn;
		snpit[5] = e1;

		/* compute point 6, back on the track */
		ypit[6] = track->distToMiddle(e3, ps(e3)->getLoc());
		snpit[6] = e3;

		/* compute spit array */
		spit[0] = 0.0;
		for (i = 1; i < PITPOINTS; i++) {
			d = 0.0;
			for (int j = snpit[i - 1]; (j + 1) % ps.Count() != snpit[i]; j++) {
				if (snpit[i] > snpit[i - 1]) {
					d = (double)(snpit[i] - snpit[i - 1]);
				}
				else {
					d = (double)(ps.Count() - snpit[i - 1] + snpit[i]);
				}
			}
			spit[i] = spit[i - 1] + d;
		}

		/* set up slopes */
		yspit[0] = pathSlope(s1);
		yspit[6] = pathSlope(e3);

		for (i = 1; i < PITPOINTS - 1; i++) {
			yspit[i] = 0.0;
		}

		/* compute path to pit */
		double l = 0.0;
		for (i = s1; (i + ps.Count()) % ps.Count() != e3; i++) {
			int j = (i + ps.Count()) % ps.Count();
			d = spline(PITPOINTS, l, spit, ypit, yspit);

			pp = track->getSegmentPtr(j)->getMiddle();
			qq = track->getSegmentPtr(j)->getToRight();

			p.x = qq->x; p.y = qq->y; p.z = 0.0;
			p.normalize();

			q.x = pp->x + p.x*d;
			q.y = pp->y + p.y*d;
			q.z = (t->pits.side == TR_LFT) ? track->getSegmentPtr(j)->getLeftBorder()->z : track->getSegmentPtr(j)->getRightBorder()->z;

			pitcord[i - s1] = q;
			ps(j)->setPitLoc(&pitcord[i - s1]);
			l += TRACKRES;
		}
	}


	/* plots pit trajectory to file for gnuplot */
	void PPathfinder::plotPitStopPath(char* filename)
	{
		FILE* fd = fopen(filename, "w");

		/* plot pit path */
		for (int i = 0; i < ps.Count(); i++) {
			fprintf(fd, "%f\t%f\n", ps(i)->getPitLoc()->x, ps(i)->getPitLoc()->y);
		}
		fclose(fd);
	}


	void PPathfinder::plotPath(char* filename)
	{
		FILE* fd = fopen(filename, "w");

		/* plot path */
		for (int i = 0; i < ps.Count(); i++) {
			fprintf(fd, "%f\t%f\n", ps(i)->getLoc()->x, ps(i)->getLoc()->y);
		}
		fclose(fd);
	}

	/*
		Updates the current static plan with new segments
	*/
	void PPathfinder::updatePlan()
	{

	}

	/*
		Plans a static route ignoring current situation
	*/
	void PPathfinder::staticPlan(PCarDesc* myc)
	{
		double r, length, speedsqr;
		int u, v, w;
		v3d dir;
		int i;

		/* Initialize location to center of the given segment */
		for (i = previousPSCount; i < ps.Count(); i++)
			ps(i)->setLoc(track->getSegmentPtr(i)->getMiddle());

#ifdef USE_CLOTHOIDS
		/* read parameter files and compute path */
		if (loadClothoidParams(cp)) {
			int i = 0, k = 0;
			while (k < ps.Count()) {
				int j = k % ps.Count();
				switch (track->getSegmentPtr(j)->getType()) {
				case TR_STR:
					i = initStraight(j, myc->CARWIDTH / 2.0 + myc->MARGIN);
					break;
				case TR_LFT:
					i = initLeft(j, myc->CARWIDTH / 2.0 + myc->MARGIN);
					break;
				case TR_RGT:
					i = initRight(j, myc->CARWIDTH / 2.0 + myc->MARGIN);
					break;
				default:
					printf("error in plan(MyCar* myc): segment is of unknown type.\n");
					break;
				}
				k = k + (i - k + ps.Count()) % ps.Count();
			}
		}
#endif // USE_CLOTHOIDS

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

		/* init pit ond optimal path */
		for (i = previousPSCount; i < ps.Count(); i++) {
			ps(i)->setOptLoc(ps(i)->getLoc());
			ps(i)->setPitLoc(ps(i)->getOptLoc());
		}

		/* compute possible speeds, direction vector and length of trajectoies */
		u = ps.Count() - 1; v = 0; w = 1;

		for (i = previousPSCount; i < ps.Count(); i++) {
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

		/* add path to pit to pit-path */
		if (isPitAvailable()) initPitStopPath();

		// Record the PS count on this call
		previousPSCount = ps.Count();
	}


	/*
		Plans a route according to the situation.
		Takes the current track segment ID? This ID is remebered for the next call, such that the plan can continue from the last point.
	*/
	void PPathfinder::dynamicPlan(int trackSegId, tCarElt* car, tSituation *situation, PCarDesc* myc, POtherCarDesc* ocar)
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

		if (track->isBetween(e3, s1, trackSegId)) inPit = false;
		/* relies on that pitstop dosen't get enabled between s1, e3 */
		if (track->isBetween(s1, e3, trackSegId) && (pitStop)) inPit = true;

		/* load precomputed trajectory */
		if (!pitStop && !inPit) {
			for (i = start; i < trackSegId + ahead + SEGRANGE; i++) {
				int j = (i + ps.Count()) % ps.Count();
				ps(j)->setLoc(ps(j)->getOptLoc());
			}
		}
		else {
			for (i = start; i < trackSegId + ahead + SEGRANGE; i++) {
				int j = (i + ps.Count()) % ps.Count();
				ps(j)->setLoc(ps(j)->getPitLoc());
			}
		}

		collcars = updateOCar(trackSegId, situation, myc, ocar, o);
		updateOverlapTimer(trackSegId, situation, myc, ocar, o, overlaptimer);

		if (!inPit && (!pitStop || track->isBetween(e3, (s1 - ahead + ps.Count()) % ps.Count(), trackSegId))) {
			/* are we on the trajectory or do i need a correction */
			if ((myc->derror > myc->PATHERR*myc->PATHERRFACTOR ||
				(myc->getDeltaPitch() > myc->MAXALLOWEDPITCH && myc->getSpeed() > myc->FLYSPEED))) {
				changed += correctPath(trackSegId, car, myc);
			}
			/* overtaking */
			if (changed == 0) {
				changed += overtake(trackSegId, situation, myc, ocar);
			}
			/* if we have nothing better to, let opponents overlap */
			if (changed == 0) {
				changed = letoverlap(trackSegId, situation, myc, ocar, overlaptimer);
			}
		}

		/* recompute speed and direction of new trajectory */
		if (changed > 0 || (ps(trackSegId)->getSpeedsqr() < 5.0)) {
			start = trackSegId;
		}

		u = start - 1; v = start; w = start + 1;
		int u2 = (start - 3 + ps.Count()) % ps.Count();
		int w2 = (start + 3 + ps.Count()) % ps.Count();
		u = (u + ps.Count()) % ps.Count();
		v = (v + ps.Count()) % ps.Count();
		w = (w + ps.Count()) % ps.Count();

		for (i = start; i < trackSegId + ahead + SEGRANGE; i++) {
			if (i > ps.Count()) break;
			int j = (i + ps.Count()) % ps.Count();
			/* taking 2 radiuses to reduce "noise" */
			double r2 = radius(ps(u)->getLoc()->x, ps(u)->getLoc()->y,
				ps(v)->getLoc()->x, ps(v)->getLoc()->y, ps(w)->getLoc()->x, ps(w)->getLoc()->y);
			double r1 = radius(ps(u2)->getLoc()->x, ps(u2)->getLoc()->y,
				ps(v)->getLoc()->x, ps(v)->getLoc()->y, ps(w2)->getLoc()->x, ps(w2)->getLoc()->y);

			if (fabs(r1) > fabs(r2)) {
				ps(j)->setRadius(r1);
				r = fabs(r1);
			}
			else {
				ps(j)->setRadius(r2);
				r = fabs(r2);
			}

			length = dist(ps(v)->getLoc(), ps(w)->getLoc());

			/* compute allowed speedsqr */
			double mu = track->getSegmentPtr(j)->getKfriction()*myc->CFRICTION*track->getSegmentPtr(j)->getKalpha();
			double b = track->getSegmentPtr(j)->getKbeta();
			speedsqr = myc->SPEEDSQRFACTOR*r*g*mu / (1.0 - MIN(1.0, (mu*myc->ca*r / myc->mass)) + mu*r*b);
			if (pitStop && track->isBetween(s3, pitSegId, j)) {
				double speedsqrpit = ((double)segmentsToPit(j) / TRACKRES) *2.0*g*track->getSegmentPtr(j)->getKfriction()*myc->CFRICTION*myc->cgcorr_b;
				if (speedsqr > speedsqrpit) speedsqr = speedsqrpit;
			}
			if ((pitStop || inPit) && track->isBetween(s3, e1, j)) {
				if (speedsqr > getPitSpeedSqrLimit()) speedsqr = getPitSpeedSqrLimit();
			}

			dir = (*ps(w)->getLoc()) - (*ps(u)->getLoc());
			dir.normalize();

			//ps(j)->set(speedsqr, length, ps(v)->getLoc(), &dir);
			ps(j)->set(speedsqr, length, &dir);

			u = v; v = w; w = (w + 1 + ps.Count()) % ps.Count();
			w2 = (w2 + 1 + ps.Count()) % ps.Count();
			u2 = (u2 + 1 + ps.Count()) % ps.Count();
		}

		changed = 0;

		/* set speed limits on the path, in case there is an obstacle (other car) */
		changed += collision(trackSegId, car, situation, myc, ocar);

		lastPlan = trackSegId; lastPlanRange = ahead;
	}


	void PPathfinder::smooth(int id, double delta, double w)
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

	void PPathfinder::smooth(int s, int p, int e, double w)
	{
		PTrackSegment* t = track->getSegmentPtr(p);
		v3d *rgh = t->getToRight();
		v3d *rs = ps(s)->getLoc(), *rp = ps(p)->getLoc(), *re = ps(e)->getLoc(), n;

		double rgx = (re->x - rs->x), rgy = (re->y - rs->y);
		double m = (rs->x * rgy + rgx * rp->y - rs->y * rgx - rp->x * rgy) / (rgy * rgh->x - rgx * rgh->y);

		n = (*rp) + (*rgh)*m;

		ps(p)->setLoc(&n);
	}


	void PPathfinder::optimize(int start, int range, double w)
	{
		for (int p = start; p < start + range; p = p + 1) {
			int j = (p) % ps.Count();
			int k = (p + 1) % ps.Count();
			int l = (p + 2) % ps.Count();
			smooth(j, k, l, w);
		}
	}


	void PPathfinder::optimize2(int start, int range, double w)
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


	void PPathfinder::optimize3(int start, int range, double w)
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


	/* collision avoidence with braking */
	int PPathfinder::collision(int trackSegId, tCarElt* mycar, tSituation* s, PCarDesc* myc, POtherCarDesc* ocar)
	{
		int end = (trackSegId + (int)COLLDIST + ps.Count()) % ps.Count();
		int didsomething = 0;
		int i, n = collcars;

		for (i = 0; i < n; i++) {
			if (o[i].overtakee == true) continue;
			int currentsegid = o[i].collcar->getCurrentSegId();
			if (track->isBetween(trackSegId, end, currentsegid) && (myc->getSpeed() > o[i].speed)) {
				int spsegid = (currentsegid - (int)(myc->CARLEN + 1) + ps.Count()) % ps.Count();

				if (o[i].mincorner < myc->CARWIDTH / 2.0 + myc->DIST) {
					double cmpdist = o[i].dist - myc->CARLEN - myc->DIST;
					if ((o[i].brakedist >= cmpdist) && (ps(spsegid)->getSpeedsqr() > o[i].speedsqr)) {
						int j;
						for (j = spsegid - 3; j < spsegid + 3; j++) {
							ps((j + ps.Count()) % ps.Count())->setSpeedsqr(o[i].speedsqr);
						}
						didsomething = 1;
					}
				}

				if (track->isBetween(trackSegId, end, o[i].catchsegid)) {
					double myd = track->distToMiddle(o[i].catchsegid, ps(o[i].catchsegid)->getLoc());
					v3d r;
					o[i].collcar->getDir()->crossProduct(myc->getDir(), &r);
					double sina = r.len()*sign(r.z);
					double otherd = o[i].disttomiddle + sina*o[i].collcar->getSpeed()*o[i].time;

					if (fabs(myd - otherd) < myc->CARWIDTH + myc->DIST) {
						if ((o[i].catchdist > 0.0) && (o[i].brakedist >= (o[i].catchdist - (myc->CARLEN + myc->DIST)))) {
							PathSeg* catchseg = getPathSeg((o[i].catchsegid - (int)myc->CARLEN + ps.Count()) % ps.Count());
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
	int PPathfinder::correctPath(int id, tCarElt* car, PCarDesc* myc)
	{
		double s[2], y[2], ys[2];
		bool out;

		double d = track->distToMiddle(id, myc->getCurrentPos());
		//	double factor = MIN(myc->CORRLEN*fabs(d), ps.Count()/2.0);
		double factor = MIN(MIN(myc->CORRLEN*myc->derror, ps.Count() / 2.0), ahead);
		int endid = (id + (int)(factor)+ps.Count()) % ps.Count();

		if (fabs(d) > (track->getSegmentPtr(id)->getWidth() - myc->CARWIDTH) / 2.0) {
			d = sign(d)*((track->getSegmentPtr(id)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN);
			ys[0] = 0.0;
			out = true;
		}
		else {
			v3d pathdir = *ps(id)->getDir();
			pathdir.z = 0.0;
			pathdir.normalize();
			double alpha = PI / 2.0 - acos((*myc->getDir())*(*track->getSegmentPtr(id)->getToRight()));
			//if (alpha > 4.0*PI/180.0) alpha = 4.0*PI/180.0;
			//if (alpha < -4.0*PI/180.0) alpha = -4.0*PI/180.0;
			ys[0] = tan(alpha);
			out = false;
		}

		double ed = track->distToMiddle(endid, ps(endid)->getLoc());


		/* set up points */
		y[0] = d;
		//ys[0] = 0.0;

		y[1] = ed;
		ys[1] = pathSlope(endid);

		s[0] = 0.0;
		s[1] = countSegments(id, endid);

		/* modify path */
		double l = 0.0;
		v3d q, *pp, *qq;
		int i, j;

		if (out == true) {
			for (i = id; (j = (i + ps.Count()) % ps.Count()) != endid; i++) {
				d = spline(2, l, s, y, ys);
				if (fabs(d) > (track->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0) {
					d = sign(d)*((track->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN);
				}

				pp = track->getSegmentPtr(j)->getMiddle();
				qq = track->getSegmentPtr(j)->getToRight();
				q = (*pp) + (*qq)*d;
				ps(j)->setLoc(&q);
				l += TRACKRES;
			}
		}
		else {
			double* newdisttomiddle = new double[ahead];
			for (i = id; (j = (i + ps.Count()) % ps.Count()) != endid; i++) {
				d = spline(2, l, s, y, ys);
				if (fabs(d) > (track->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN) {
					return 0;
				}
				newdisttomiddle[i - id] = d;
				l += TRACKRES;
			}

			for (i = id; (j = (i + ps.Count()) % ps.Count()) != endid; i++) {
				pp = track->getSegmentPtr(j)->getMiddle();
				qq = track->getSegmentPtr(j)->getToRight();
				q = *pp + (*qq)*newdisttomiddle[i - id];
				ps(j)->setLoc(&q);
			}

			delete[] newdisttomiddle;
		}

		/* align previos point for getting correct speedsqr in PPathfinder::plan(...) */
		int p = (id - 1 + ps.Count()) % ps.Count();
		int e = (id + 1 + ps.Count()) % ps.Count();
		smooth(id, p, e, 1.0);

		return 1;
	}


	/* compute path for overtaking the "next colliding" car */
	int PPathfinder::overtake(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar)
	{
		if (collcars == 0) return 0;

		const int start = (trackSegId - (int)(2.0 + myc->CARLEN) + ps.Count()) % ps.Count();
		const int nearend = (trackSegId + (int)(2.0*myc->CARLEN)) % ps.Count();

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
				if (dst < minorthdist && track->isBetween(start, nearend, o[i].collcar->getCurrentSegId())) {
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
				if (track->getSegmentPtr((trackSegId + i) % ps.Count())->getRadius() < myc->OVERTAKERADIUS) return 0;
			}
		}
		else return 0;

		/* not enough space, so we try to overtake */
		if (((o[collcarindex].mincorner < myc->CARWIDTH / 2.0 + myc->DIST) && (minTime < myc->TIMETOCATCH)) || !sidechangeallowed) {
			int overtakerange = (int)MIN(MAX((3 * minTime*myc->getSpeed()), myc->MINOVERTAKERANGE), ahead - 50);
			double d = o[collcarindex].disttomiddle;
			double mydisttomiddle = track->distToMiddle(myc->getCurrentSegId(), myc->getCurrentPos());
			double y[3], ys[3], s[3];

			y[0] = track->distToMiddle(trackSegId, myc->getCurrentPos());
			double alpha = PI / 2.0 - acos((*myc->getDir())*(*track->getSegmentPtr(trackSegId)->getToRight()));
			int trackSegId1 = (trackSegId + overtakerange / 3) % ps.Count();
			double w = track->getSegmentPtr(nearestCar->getCurrentSegId())->getWidth() / 2;
			double pathtomiddle = track->distToMiddle(trackSegId1, ps(trackSegId1)->getLoc());
			double paralleldist = o[collcarindex].cosalpha*dist(myc->getCurrentPos(), nearestCar->getCurrentPos());

			if (!sidechangeallowed) {
				if (paralleldist > 1.5*myc->CARLEN) {
					int i;
					for (i = 0; i <= (int)myc->MINOVERTAKERANGE; i += 10) {
						if (track->getSegmentPtr((trackSegId + i) % ps.Count())->getRadius() < myc->OVERTAKERADIUS) return 0;
					}
					v3d r, dir = *o[collcarindex].collcar->getCurrentPos() - *myc->getCurrentPos();
					myc->getDir()->crossProduct(&dir, &r);
					double pathtocarsgn = sign(r.z);

					y[1] = d + myc->OVERTAKEDIST*pathtocarsgn;
					if (fabs(y[1]) > w - (1.5*myc->CARWIDTH)) {
						y[1] = d - myc->OVERTAKEDIST*pathtocarsgn;
					}
					double beta = PI / 2.0 - acos((*nearestCar->getDir())*(*track->getSegmentPtr(trackSegId)->getToRight()));
					if (y[1] - mydisttomiddle >= 0.0) {
						if (alpha < beta + myc->OVERTAKEANGLE) alpha = alpha + myc->OVERTAKEANGLE;
					}
					else {
						if (alpha > beta - myc->OVERTAKEANGLE) alpha = alpha - myc->OVERTAKEANGLE;
					}
				}
				else {
					double beta = PI / 2.0 - acos((*nearestCar->getDir())*(*track->getSegmentPtr(trackSegId)->getToRight()));
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
			int trackSegId2 = (trackSegId + overtakerange) % ps.Count();
			y[2] = track->distToMiddle(trackSegId2, ps(trackSegId2)->getOptLoc());
			ys[2] = pathSlope(trackSegId2);

			/* set up parameter s */
			s[0] = 0.0;
			s[1] = countSegments(trackSegId, trackSegId1);
			s[2] = s[1] + countSegments(trackSegId1, trackSegId2);

			/* check path for leaving to track */
			double* newdisttomiddle = new double[ahead];
			int i, j;
			double l = 0.0; v3d q, *pp, *qq;
			for (i = trackSegId; (j = (i + ps.Count()) % ps.Count()) != trackSegId2; i++) {
				d = spline(3, l, s, y, ys);
				if (fabs(d) > (track->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN) {
					o[collcarindex].overtakee = false;
					return 0;
				}
				newdisttomiddle[i - trackSegId] = d;
				l += TRACKRES;
			}

			/* set up the path */
			for (i = trackSegId; (j = (i + ps.Count()) % ps.Count()) != trackSegId2; i++) {
				pp = track->getSegmentPtr(j)->getMiddle();
				qq = track->getSegmentPtr(j)->getToRight();
				q = *pp + (*qq)*newdisttomiddle[i - trackSegId];
				ps(j)->setLoc(&q);
			}

			delete[] newdisttomiddle;

			/* reload old trajectory where needed */
			for (i = trackSegId2; (j = (i + ps.Count()) % ps.Count()) != (trackSegId + ahead) % ps.Count(); i++) {
				ps(j)->setLoc(ps(j)->getOptLoc());
			}

			/* align previos point for getting correct speedsqr in PPathfinder::plan(...) */
			int p = (trackSegId - 1 + ps.Count()) % ps.Count();
			int e = (trackSegId + 1 + ps.Count()) % ps.Count();
			smooth(trackSegId, p, e, 1.0);

			return 1;
		}
		else {
			return 0;
		}
	}


	/* collect data about other cars relative to me */
	inline int PPathfinder::updateOCar(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o)
	{
		const int start = (trackSegId - (int)(1.0 + myc->CARLEN / 2.0) + ps.Count()) % ps.Count();
		const int end = (trackSegId + (int)COLLDIST + ps.Count()) % ps.Count();

		int i, n = 0;		/* counter for relevant cars */

		for (i = 0; i < s->_ncars; i++) {
			tCarElt* car = ocar[i].getCarPtr();
			/* is it me ? */
			if (car != myc->getCarPtr()) {
				int seg = ocar[i].getCurrentSegId();
				/* get the next car to catch up */
				if (track->isBetween(start, end, seg) && !(car->_state & (RM_CAR_STATE_DNF | RM_CAR_STATE_PULLUP | RM_CAR_STATE_PULLSIDE | RM_CAR_STATE_PULLDN))) {
					o[n].cosalpha = (*myc->getDir())*(*ocar[i].getDir());
					o[n].speed = ocar[i].getSpeed()*o[n].cosalpha;
					int j, k = track->diffSegId(trackSegId, seg);
					if (k < 40) {
						o[n].dist = 0.0;
						int l = MIN(trackSegId, seg);
						for (j = l; j < l + k; j++) o[n].dist += ps(j % ps.Count())->getLength();
						if (o[n].dist > k) o[n].dist = k;
					}
					else {
						o[n].dist = k;
					}
					o[n].collcar = &ocar[i];
					o[n].time = o[n].dist / (myc->getSpeed() - o[n].speed);
					o[n].disttomiddle = track->distToMiddle(seg, ocar[i].getCurrentPos());
					o[n].speedsqr = sqr(o[n].speed);
					o[n].catchdist = (int)(o[n].dist / (myc->getSpeed() - ocar[i].getSpeed())*myc->getSpeed());
					o[n].catchsegid = (o[n].catchdist + trackSegId + ps.Count()) % ps.Count();
					o[n].overtakee = false;
					o[n].disttopath = distToPath(seg, ocar[i].getCurrentPos());
					double gm = track->getSegmentPtr(seg)->getKfriction()*myc->CFRICTION;
					double qs = o[n].speedsqr;
					o[n].brakedist = (myc->getSpeedSqr() - o[n].speedsqr)*(myc->mass / (2.0*gm*g*myc->mass + (qs)*(gm*myc->ca)));
					o[n].mincorner = FLT_MAX;
					o[n].minorthdist = FLT_MAX;
					for (j = 0; j < 4; j++) {
						v3d e(car->pub.corner[j].ax, car->pub.corner[j].ay, car->_pos_Z);
						double corner = fabs(distToPath(seg, &e));
						double orthdist = track->distGFromPoint(myc->getCurrentPos(), myc->getDir(), &e) - myc->CARWIDTH / 2.0;
						if (corner < o[n].mincorner) o[n].mincorner = corner;
						if (orthdist < o[n].minorthdist) o[n].minorthdist = orthdist;
					}
					n++;
				}
			}
		}
		return n;
	}


	inline void PPathfinder::updateOverlapTimer(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o, tOverlapTimer* ov)
	{
		const int start = (trackSegId - (int)myc->OVERLAPSTARTDIST + ps.Count()) % ps.Count();
		const int end = (trackSegId - (int)(2.0 + myc->CARLEN / 2.0) + ps.Count()) % ps.Count();
		const int startfront = (trackSegId + (int)(2.0 + myc->CARLEN / 2.0)) % ps.Count();
		const int endfront = (trackSegId + (int)myc->OVERLAPSTARTDIST) % ps.Count();

		int i;

		for (i = 0; i < s->_ncars; i++) {
			tCarElt* car = ocar[i].getCarPtr();
			tCarElt* me = myc->getCarPtr();
			/* is it me, and in case not, has the opponent more laps than me? */
			if ((car != me) && (car->race.laps > me->race.laps) &&
				!(car->_state & (RM_CAR_STATE_DNF | RM_CAR_STATE_PULLUP | RM_CAR_STATE_PULLSIDE | RM_CAR_STATE_PULLDN))) {
				int seg = ocar[i].getCurrentSegId();
				if (track->isBetween(start, end, seg)) {
					ov[i].time += s->deltaTime;
				}
				else if (track->isBetween(startfront, endfront, seg)) {
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
	int PPathfinder::letoverlap(int trackSegId, tSituation *situation, PCarDesc* myc, POtherCarDesc* ocar, tOverlapTimer* ov)
	{
		const int start = (trackSegId - (int)myc->OVERLAPPASSDIST + ps.Count()) % ps.Count();
		const int end = (trackSegId - (int)(2.0 + myc->CARLEN / 2.0) + ps.Count()) % ps.Count();
		int k;

		for (k = 0; k < situation->_ncars; k++) {
			if ((ov[k].time > myc->OVERLAPWAITTIME) && track->isBetween(start, end, ocar[k].getCurrentSegId())) {
				/* let overtake */
				double s[4], y[4], ys[4];
				const int DST = 400;

				ys[0] = pathSlope(trackSegId);
				if (fabs(ys[0]) > PI / 180.0) return 0;

				int trackSegId1 = (trackSegId + (int)DST / 4 + ps.Count()) % ps.Count();
				int trackSegId2 = (trackSegId + (int)DST * 3 / 4 + ps.Count()) % ps.Count();
				int trackSegId3 = (trackSegId + (int)DST + ps.Count()) % ps.Count();
				double width = track->getSegmentPtr(trackSegId1)->getWidth();

				/* point 0 */
				y[0] = track->distToMiddle(trackSegId, myc->getCurrentPos());

				/* point 1 */
				y[1] = sign(y[0])*MIN((width / 2.0 - 2.0*myc->CARWIDTH - myc->MARGIN), (15.0 / 2.0));
				ys[1] = 0.0;

				/* point 2 */
				y[2] = y[1];
				ys[2] = 0.0;

				/* point 3*/
				y[3] = track->distToMiddle(trackSegId3, ps(trackSegId3)->getOptLoc());
				ys[3] = pathSlope(trackSegId3);

				/* set up parameter s */
				s[0] = 0.0;
				s[1] = countSegments(trackSegId, trackSegId1);
				s[2] = s[1] + countSegments(trackSegId1, trackSegId2);
				s[3] = s[2] + countSegments(trackSegId2, trackSegId3);

				/* check path for leaving to track */
				double* newdisttomiddle = new double[ahead];
				double d;
				int i, j;
				double l = 0.0; v3d q, *pp, *qq;
				for (i = trackSegId; (j = (i + ps.Count()) % ps.Count()) != trackSegId3; i++) {
					d = spline(4, l, s, y, ys);
					if (fabs(d) > (track->getSegmentPtr(j)->getWidth() - myc->CARWIDTH) / 2.0 - myc->MARGIN) {
						return 0;
					}
					newdisttomiddle[i - trackSegId] = d;
					l += TRACKRES;
				}

				/* set up the path */
				for (i = trackSegId; (j = (i + ps.Count()) % ps.Count()) != trackSegId3; i++) {
					pp = track->getSegmentPtr(j)->getMiddle();
					qq = track->getSegmentPtr(j)->getToRight();
					q = *pp + (*qq)*newdisttomiddle[i - trackSegId];
					ps(j)->setLoc(&q);
				}

				delete [] newdisttomiddle;

				/* reload old trajectory where needed */
				for (i = trackSegId3; (j = (i + ps.Count()) % ps.Count()) != (trackSegId + ahead) % ps.Count(); i++) {
					ps(j)->setLoc(ps(j)->getOptLoc());
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

	void PPathfinder::Update(tSituation* situation)
	{
		stateMngr.Update();

		if (stateMngr.IsUpdateNeeded())
		{
			// Calculate the number of path segments that need to be added
			int nNewSegs = track->segmentCount() - ps.Count();

			// Create an array of path segments equal to the size of the number of new segments needed
			PathSegCollection newSegs = PathSegCollection(nNewSegs);

			// Append the collection of path segments
			ps.Append(newSegs.Segments());

			// Calculate distance to look ahead
			ahead = track->GetTorcsTrack()->length - getCurrentSegment(car);;

			// Compute a new static plan
			staticPlan(carDesc);
		}
	}
}