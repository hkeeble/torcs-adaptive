/*
	File: PTrackSegment.h
	Author: Henri Keeble
	Desc: A class used to store a collection of approximtely 1 meter long track segments.
*/

#ifndef _P_TRACK_SEGMENT_COLLECTION_H_
#define _P_TRACK_SEGMENT_COLLECTION_H_

#include "PTrackSegment.h"

namespace procBot
{
	/* Class to represent a collection of track segments, used inside of PTrackDesc */
	class PTrackSegmentCollection
	{
	private:
		PTrackSegment* segs;
		int nSegs;

		// Internal copying function
		void cpy(const PTrackSegmentCollection& param);
	public:
		PTrackSegmentCollection() : segs(nullptr), nSegs(0) { }
		PTrackSegmentCollection(PTrackSegment* segs, int numberOfSegs);
		PTrackSegmentCollection(const PTrackSegmentCollection& param);
		PTrackSegmentCollection& operator=(const PTrackSegmentCollection& param);
		~PTrackSegmentCollection();

		/* Used to index specific segments */
		PTrackSegment* operator()(int index);

		/* Returns the number of segments being stored */
		int Length() const;

		/* Appends the collection with the given new segments */
		void Append(PTrackSegment* newSegs, int nOfNewSegs);
	};
}

#endif // _P_TRACK_SEGMENT_COLLECTION_H_