/*
	File: PathSeg.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Modified version of the Pathfinder from the TORCS robot berniw for a procedural track.
*/

#ifndef _PATH_SEG_H_
#define _PATH_SEG_H_

#include "linalg.h"
#include "tgf.h"

namespace procBot
{
	/* Represents a single segment of the planned path */
	class PathSeg
	{
		public:
			void set(tdble ispeedsqr, tdble ilength, v3d* ip, v3d* id);
			void set(tdble ispeedsqr, tdble ilength, v3d* id);
			inline void setLoc(v3d* ip) { p = (*ip); }
			inline void setOptLoc(v3d* ip) { o = (*ip); }
			inline void setPitLoc(v3d* ip) { l = ip; }

			inline void setSpeedsqr(tdble spsqr) { speedsqr = spsqr; }
			inline void setWeight(tdble w) { weight = w; }
			inline void setRadius(tdble r) { radius = r; }

			inline tdble getSpeedsqr() { return speedsqr; }
			inline tdble getLength() { return length; }
			inline tdble getWeight() { return weight; }
			inline tdble getRadius() { return radius; }

			inline v3d* getOptLoc() { return &o; }
			inline v3d* getPitLoc() { return l; }
			inline v3d* getLoc() { return &p; }
			inline v3d* getDir() { return &d; }

		private:
			tdble speedsqr;	/* max possible speed sqared (speed ist therefore sqrt(speedsqr) */
			tdble length;	/* dist to the next pathseg */
			tdble weight;	/* weight function value for superposition */
			tdble radius;	/* radius of current segment */
			v3d p;			/* position in space, dynamic trajectory */
			v3d o;			/* position in space, static trajectory */
			v3d d;			/* direction vector of dynamic trajectory */
			v3d* l;			/* trajectory for pit lane */
	};
}

#endif // _PATH_SEG_H_