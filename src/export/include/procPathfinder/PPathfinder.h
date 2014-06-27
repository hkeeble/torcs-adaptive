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

namespace procPathfinder
{	
	static const double g = 9.81;

	#define FNPF "drivers/berniProc/parameter.dat"
	#define FNIS "drivers/berniProc/intsinsqr.dat"
	#define FNIC "drivers/berniProc/intcossqr.dat"

	/* how many segments can i pass per simulation step, depends on TRACKRES, simulation->_deltaTime and speed */
	#define SEGRANGE 3

	class PCarDesc;
	class POtherCarDesc;

	/* The PPathfinder class, holds and manages an array of path segments */
	class PPathfinder
	{
		public:
			PPathfinder(PTrackDesc* itrack, PCarDesc* carDesc, tSituation *situation);
			~PPathfinder();

			/*!< Plan the path. Override in base classes. */
			virtual void Plan(PCarDesc* myc) = 0;

			/*!< Used to update the pathfinder when new track exists. */
			void Update(tSituation* situation);

			/*!< Plots the current path to the optimal file. */
			void PlotPath(char* filename);

			// Common Computations
			inline double sqr(double a) { return a*a; };
			inline double dist(v3d* a, v3d* b) { return sqrt(sqr(a->x-b->x) + sqr(a->y-b->y) + sqr(a->z-b->z)); }
			inline double dist2D(v3d* a, v3d* b) { return sqrt(sqr(a->x-b->x) + sqr(a->y-b->y)); }
			double distToPath(int trackSegId, v3d* p);

			// Segment Accessors
			inline int getnPathSeg() { return ps.Count(); }
			int getCurrentSegment(tCarElt* car);
			int getCurrentSegment(tCarElt* car, int range);

			// Accessors
			PTrackDesc* Track() const;  
			PCarDesc* CarDesc() const;
			PStateManager PState() const;
			tCarElt* Car() const;
			int LookAhead() const;

			/* Operator overload, returns path segment at given index. */
			PathSeg* Seg(int index);

		protected:
			static const double TPRES;		/* resolution of the steps */
			enum { NTPARAMS = 1001 };		/* # entries in dat files */

			PTrackDesc* track;		 /* pointer to track data */
			PCarDesc* carDesc;		 /* pointer to procedural car description */
			PStateManager stateMngr; /* State manager for the pathfinder */
			tCarElt* car;			 /* Pointer to torcs car structure */

			int previousPSCount;	/* Previous path segment count on last call to static plan */

			int lastId;				/* segment id of the last call */
			PathSegCollection ps;	/* collection of path segments with the plan */

			int ahead;				/* Distance, in meters, to look ahead */
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
}

#endif // _P_PPATHFINDER_H_