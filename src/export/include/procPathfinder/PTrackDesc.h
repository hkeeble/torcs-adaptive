/*
	File: PPTrackDesc.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Modified version of PTrackDesc class from the TORCS robot berniw for a procedural track.
*/

#ifndef _P_TRACK_DESC_H_
#define _P_TRACK_DESC_H_

#include "track.h"
#include "PTrackSegment.h"
#include "car.h"
#include "PStateManager.h"
#include "PTrackSegmentCollection.h"

namespace procPathfinder
{
	#define TRACKRES 1.0
	#define RMAX 10000.0

	/* magic radius for bumpy roads */
	#define RREL 180.0

	class PTrackDesc
	{
		public:
			PTrackDesc(const tTrack* track);
			~PTrackDesc();

			/* output track data to gnuplot readable file */
			void plot(char* filename);

			/* outputs the curvature of each point on the track */
			void outputCurvature(char* filename);

			inline tTrack* GetTorcsTrack() { return stateMngr.GetTorcsTrack(); }
			inline PTrackSegment* getSegmentPtr(int index) { return ts(index); }
			inline int segmentCount() { return ts.Count(); }
			int getCurrentSegment(tCarElt* car, int lastId, int range);
			int getCurrentSegment(tCarElt* car);
			int getNearestId(v3d* p);

			inline int getPitEntryStartId() { return nPitEntryStart; }
			inline int getPitExitEndId() { return nPitExitEnd; }
			inline int getPitType() { return stateMngr.GetTorcsTrack()->pits.type; }

			static inline void dirVector2D(v3d* a, v3d* b, v3d* r) { r->x = a->x - b->x; r->y = a->y - b->y; r->z = 0.0; }
			static inline double cosalpha(v3d* a, v3d* b) { return (*a)*(*b)/(a->len()*b->len()); }
			double distGFromPoint(v3d* r1, v3d* rdir, v3d* p);

			/* returns distance to middle with p on the toright vector: value > 0 is right, value < 0 is left */
			inline double distToMiddle(int id, v3d* p) { return (*p - *ts(id)->getMiddle())*(*ts(id)->getToRight()); }
			bool isBetween(int start, int end, int id);
			int diffSegId(int id1, int id2);

			// Updates the track description based on state
			void Update();

		private:
			PStateManager stateMngr; // Current state of the track description
			PTrackSegmentCollection ts; // Collection of track segments

			int nPitEntryStart;
			int nPitExitEnd;
	};

	inline double PTrackDesc::distGFromPoint(v3d* r1, v3d* rdir, v3d* p) {
		v3d t, s;
		p->dirVector(r1, &t);
		rdir->crossProduct(&t, &s);
		return s.len() / rdir->len();
	}


	inline bool PTrackDesc::isBetween(int start, int end, int id) {
		if (start <= end) {
			if (id >= start && id <= end) { return true; }
			else { return false; }
		}
		else {
			if ((id >= 0 && id <= end) || (id >= start && id < ts.Count())) { return true; }
			else { return false; }
		}
	}


	inline int PTrackDesc::diffSegId(int id1, int id2) {
		int t;
		if (id1 > id2) { t = id1; id1 = id2; id2 = t; }
		t = (ts.Count() - id2 + id1) % ts.Count();
		return MIN(id2 - id1, t);
	}


	/* get the segment on which the car is, searching from the position of the last call within range */
	inline int PTrackDesc::getCurrentSegment(tCarElt* car, int lastId, int range)
	{
		int start = -(range / 4);
		int end = range * 3 / 4;
		double d, min = FLT_MAX;
		PTrackSegment* ts;
		int minindex = 0;

		for (int i = start; i < end; i++) {
			int j = (lastId + i + segmentCount()) % segmentCount();
			ts = getSegmentPtr(j);
			d = ts->distToMiddleSqr3D(car->_pos_X, car->_pos_Y, car->_pos_Z);
			if (d < min) {
				min = d;
				minindex = j;
			}
		}
		return minindex;
	}

}

#endif // _P_TRACK_DESC_H_