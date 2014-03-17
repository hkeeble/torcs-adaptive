/*
	File: PTrackSegment.h
	Author: Henri Keeble
	Desc: A class used to store a collection of approximtely 1 meter long track segments.
*/

#ifndef _P_TRACK_SEGMENT_COLLECTION_H_
#define _P_TRACK_SEGMENT_COLLECTION_H_

#include "PTrackSegment.h"
#include <vector>

using std::vector;

namespace procBot
{
	/* Class to represent a collection of track segments, used inside of PTrackDesc */
	class PTrackSegmentCollection
	{
	private:
		vector<PTrackSegment> segs;

		// Internal copying function
		void cpy(const PTrackSegmentCollection& param);
	public:
		PTrackSegmentCollection() : segs(vector<PTrackSegment>()) { }
		PTrackSegmentCollection(const vector<PTrackSegment>& segs);
		PTrackSegmentCollection(const PTrackSegmentCollection& param);
		PTrackSegmentCollection& operator=(const PTrackSegmentCollection& param);
		~PTrackSegmentCollection();

		/* Used to index specific segments */
		PTrackSegment* operator()(int index);

		/* Returns the number of segments being stored */
		int Count() const;

		/* Appends the collection with the given new segments */
		void Append(const vector<PTrackSegment>& newSegs);

#ifdef _DEBUG
		/* Debug only function. Prints all segment widths - used to check validity of data stored */
		void PrintAll()
		{
			for (int i = 0; i < Count(); i++)
				std::cout << segs[i].getTrackSegment()->id << std::endl;
			std::cout << "Total: " << Count() << std::endl;
		}
#endif // _DEBUG
	};
}

#endif // _P_TRACK_SEGMENT_COLLECTION_H_