/*
	File: PPPathfinder.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Modified version of the Pathfinder from the TORCS robot berniw for a procedural track.
*/

#ifndef _P_PPATHFINDER_H_
#define _P_PPATHFINDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robot.h>
#include <robottools.h>
#include <math.h>
#include "PTrackDesc.h"
#include "PCarDesc.h"
#include "PathSegCollection.h"

namespace procBot
{	
	static const double g = 9.81;

	#define FNPF "drivers/berniProc/parameter.dat"
	#define FNIS "drivers/berniProc/intsinsqr.dat"
	#define FNIC "drivers/berniProc/intcossqr.dat"

	/* how many segments can i pass per simulation step, depends on TRACKRES, simulation->_deltaTime and speed */
	#define SEGRANGE 3

	class PCarDesc;
	class POtherCarDesc;

	// #define PATH_K1999
	#define PATH_BERNIW
	
	/* holds a point of datafile from clothoid */
	typedef struct {
		double x;
		double pd;
		double is;
		double ic;
	} tParam;

	/* holds data relative to my car */
	typedef struct {
		double speedsqr;		/* on track direction projected speed squared of opponent */
		double speed;			/* same, but not squared */
		double time;			/* estimate of time to catch up the car */
		double cosalpha;		/* cos(alpha) from angle between my ond opponent */
		double disttomiddle;	/* distance to middle (for prediction) */
		int catchdist;
		int catchsegid;			/* segment, where i expect (or better guess!) to catch opponent */
		double dist;			/* #segments from me to the other car */
		POtherCarDesc* collcar;		/* pointers to the cars */
		bool overtakee;			/* is this the guy to overtake? */
		double disttopath;		/* distance to my path */
		double brakedist;		/* distance needed for braking to the speed of this car */
		double mincorner;		/* corner nearest to my car */
		double minorthdist;		/* minimal distance relative to my car */
	} tOCar;


	/* holds data needed for let pass opponents */
	typedef struct {
		double time;			/* how long is the opponent "in range" to overlap me */
	} tOverlapTimer;

	/* The PPathfinder class, holds and manages an array of path segments */
	class PPathfinder
	{
		public:
			PPathfinder(PTrackDesc* itrack, PCarDesc* carDesc, tSituation *situation);
			~PPathfinder();
			void dynamicPlan(int trackSegId, tCarElt* car, tSituation* situation, PCarDesc* myc, POtherCarDesc* ocar);
			void staticPlan(PCarDesc* myc);

			/* Used to update the PPPathfinder when neccesary */
			void Update(tSituation* situation);

			void initPit(tCarElt* car);
			inline bool isPitAvailable() { return pit; }
			inline int getPitSegId() { return pitSegId; }
			void setPitStop(bool p, int id);
			inline bool getPitStop() { return pitStop; }
			int segmentsToPit(int id);
			void plotPitStopPath(char* filename);
			void plotPath(char* filename);

			inline double sqr(double a) { return a*a; };
			inline double dist(v3d* a, v3d* b) { return sqrt(sqr(a->x-b->x) + sqr(a->y-b->y) + sqr(a->z-b->z)); }
			inline double dist2D(v3d* a, v3d* b) { return sqrt(sqr(a->x-b->x) + sqr(a->y-b->y)); }
			inline PathSeg* getPathSeg(int pathSegId) { return ps(pathSegId); }
			int getCurrentSegment(tCarElt* car);
			int getCurrentSegment(tCarElt* car, int range);
			inline int getnPathSeg() { return ps.Count(); }
			inline double getPitSpeedSqrLimit() { return pitspeedsqrlimit; }
			double distToPath(int trackSegId, v3d* p);

		private:
			static const double COLLDIST;	/* up to this distance do we consider other cars as dangerous */
			static const double TPRES;		/* resolution of the steps */
			enum { PITPOINTS = 7 };			/* # points for pit spline */
			enum { NTPARAMS = 1001 };		/* # entries in dat files */
			tParam cp[NTPARAMS];			/* holds values needed for clothiod */

			PTrackDesc* track;		 /* pointer to track data */
			PCarDesc* carDesc;		 /* pointer to procedural car description */
			PStateManager stateMngr; /* State manager for the pathfinder */
			tCarElt* car;			 /* pointer to torcs car structure */

			int previousPSCount; /* Previous path segment count on last call to static plan */

			int lastId;				/* segment id of the last call */
			PathSegCollection ps;	/* collection of path segments with the plan */
			int lastPlan;			/* start of the last plan */
			int lastPlanRange;		/* range of the last plan */
			bool pitStop;			/* pitstop ? */
			bool inPit;				/* internal pit state */

			int s1, s3;				/* pitentrystart, pitentryend */
			int e1, e3;				/* pitexitstart, pitexitend */

			v3d pitLoc;				/* location of pit */
			int pitSegId;			/* segment id of pit */
			bool pit;
			int changed;
			double pitspeedsqrlimit;	/* speed limit for pit lane squared */

			int ahead; /* Distance, in meters, to look ahead */

			int collcars;
			tOCar* o;
			tOverlapTimer* overlaptimer;
			v3d* pitcord;

			/* Initialize the pathfinder */
			void Init(tSituation* situation);

			void initPitStopPath(void);
			void getPitPoint(int j, int k, double slope, double dist, v3d* r);
			int collision(int trackSegId, tCarElt* mycar, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar);
			int overtake(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar);
			double curvature(double xp, double yp, double x, double y, double xn, double yn);
			void adjustRadius(int s, int p, int e, double c, double carwidth);
			void stepInterpolate(int iMin, int iMax, int Step);
			void interpolate(int Step);
			void smooth(int Step);

			int correctPath(int id, tCarElt* car, PCarDesc* myc);

			/* Original pathfinding functions from berniw robot */
#ifdef PATH_BERNIW
			bool loadClothoidParams(tParam* p);
			double intsinsqr(double alpha);
			double intcossqr(double alpha);
			double clothparam(double alpha);
			double clothsigma(double beta, double y);
			double clothlength(double beta, double y);
#endif PATH_BERNIW

			int findStartSegId(int id);
			int findEndSegId(int id);
			int initStraight(int id, double w);
			int initLeft(int id, double w);
			int initRight(int id, double w);
			double computeWeight(double x, double len);
			void setLocWeighted(int id, double newweight, v3d* newp);
			void smooth(int s, int e, int p, double w);
			void smooth(int id, double delta, double w);
			void optimize(int start, int range, double w);
			void optimize2(int start, int range, double w);
			void optimize3(int start, int range, double w);
			int updateOCar(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o);
			void updateOverlapTimer(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o, tOverlapTimer* ov);
			int letoverlap(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOverlapTimer* ov);
			double pathSlope(int id);
			int countSegments(int from, int to);
	};

	/* Used to calculate the distance from the given path. */
	inline double PPathfinder::distToPath(int trackSegId, v3d* p)
	{
		// Obtain direction vector to the right of the current segment
		v3d *toright = track->getSegmentPtr(trackSegId)->getToRight();

		// Obtain the desired direction for the current path segment
		v3d *pathdir = ps(trackSegId)->getDir();

		v3d n1, torightpath;

		// Obtain cross product of right vector and path direction
		toright->crossProduct(pathdir, &n1);

		// Obtain cross product of previous value and path direction
		pathdir->crossProduct(&n1, &torightpath);

		// Return current position - current desired location * distance to right path / length of right path
		return ((*p - *ps(trackSegId)->getLoc())*torightpath) / torightpath.len();
	}


	inline void PathSeg::set(tdble ispeedsqr, tdble ilength, v3d* ip, v3d* id) {
		speedsqr = ispeedsqr;
		length = ilength;
		p = (*ip);
		d = (*id);
	}


	inline void PathSeg::set(tdble ispeedsqr, tdble ilength, v3d* id) {
		speedsqr = ispeedsqr;
		length = ilength;
		d = (*id);
	}


	inline void PPathfinder::setPitStop(bool p, int id) {
		if (isPitAvailable() && track->isBetween(e3, (s1 - ahead + ps.Count()) % ps.Count(), id) && p) {
			pitStop = true ;
		} else {
			pitStop = false;
		}
	}


	inline int PPathfinder::segmentsToPit(int id) {
		if (id <= pitSegId) {
			return (pitSegId - id);
		} else {
			return (track->segmentCount() - id + pitSegId);
		}
	}


	inline double PPathfinder::pathSlope(int id) {
		int nextid = (id + 1) % ps.Count();
		v3d dir = *ps(nextid)->getLoc() - *ps(id)->getLoc();
		double dp = dir*(*track->getSegmentPtr(id)->getToRight())/dir.len();
		double alpha = PI/2.0 - acos(dp);
		return tan(alpha);
	}


	/* get the segment on which the car is, searching ALL the segments */
	inline int PPathfinder::getCurrentSegment(tCarElt* car)
	{
		lastId = track->getCurrentSegment(car);
		return lastId;
	}


	/* get the segment on which the car is, searching from the position of the last call within range */
	inline int PPathfinder::getCurrentSegment(tCarElt* car, int range)
	{
		lastId = track->getCurrentSegment(car, lastId, range);
		return lastId;
	}


	inline int PPathfinder::countSegments(int from, int to)
	{
		if ( to >= from) {
			return to - from;
		} else {
			return ps.Count() - from + to;
		}
	}
}

#endif // _P_PPATHFINDER_H_