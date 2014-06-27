/*
	File: PPTrackSegment.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Modified version of TrackSegment from the TORCS robot berniw for a procedural track.
*/

#ifndef _P_TRACK_SEGMENT_H_
#define _P_TRACK_SEGMENT_H_

#include <track.h>
#include "taMath/taMath.h"

namespace procBot
{
	class PTrackSegment
	{
		public:
			PTrackSegment();
			PTrackSegment(int id, const tTrackSeg* s, const v3d* l, const v3d* m, const v3d* r);
			~PTrackSegment();
			inline void setLength(double len) { length = len; }
			inline void setKbeta(double b) { kbeta = b; }
			inline void setKgamma(double c) { kgamma = c; }

			inline int getType() { return type; }
			inline unsigned int getRaceType() { return raceType; }
			inline tdble getRadius() { return radius; }

			inline tTrackSeg* getTrackSegment() { return pTrackSeg; }

			inline tdble getKfriction() { return pTrackSeg->surface->kFriction; }
			inline tdble getKrollres() { return pTrackSeg->surface->kRollRes; }
			inline tdble getKroughness() { return pTrackSeg->surface->kRoughness; }
			inline tdble getKroughwavelen() { return pTrackSeg->surface->kRoughWaveLen; }

			inline tdble getWidth() { return width; }
			inline tdble getKalpha() { return kalpha; }
			inline tdble getKbeta() { return kbeta; }
			inline tdble getKgamma() { return kgamma; }
			inline tdble getLength() { return length; }

			inline v3d* getLeftBorder() { return &l; }
			inline v3d* getRightBorder() { return &r; }
			inline v3d* getMiddle() { return &m; }
			inline v3d* getToRight() { return &tr; }

			inline double sqr(double a) { return a*a; }
			inline double distToMiddle2D(double x, double y) { return sqrt(sqr(x-m.x) + sqr(y-m.y)); }

			inline double distToRight3D(v3d* p) { return sqrt(sqr(p->x-r.x) + sqr(p->y-r.y) + sqr(p->z-r.z)); }
			inline double distToLeft3D(v3d* p) { return sqrt(sqr(p->x-l.x) + sqr(p->y-l.y) + sqr(p->z-l.z)); }
			inline double distToMiddle3D(double x, double y, double z) { return sqrt(sqr(x-m.x) + sqr(y-m.y) + sqr(z-m.z)); }
			inline double distToMiddleSqr3D(double x, double y, double z) { return sqr(x-m.x) + sqr(y-m.y) + sqr(z-m.z); }
			inline double distToMiddle3D(v3d* p) { return sqrt(sqr(p->x-m.x) + sqr(p->y-m.y) + sqr(p->z-m.z)); }

		private:
			tTrackSeg* pTrackSeg;	/* id of the corresponding segment */
			int type;				/* physical type (eg. straight, left or right) */
			unsigned int raceType;	/* race type (eg. pitlane, speedlimit, ...) */
			v3d l, m, r;			/* right, middle and left segment (road) border */
			v3d tr;					/* normalized direction vector to the right side */
			tdble radius;			/* radius */
			tdble width;			/* width of the track segment*/
			tdble kalpha;			/* (roll)factor for the angle (like michigan) */
			tdble kbeta;			/* (curvature)factor for bumps (e-track-3) */
			tdble kgamma;			/* (pitch)factor of road */
			tdble length;			/* distance to the next segment (2-D, not 3-D!) */
	};
}

#endif // _P_TRACK_SEGMENT_H_