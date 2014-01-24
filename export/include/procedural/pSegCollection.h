/*
	File: pSegCollection.h
	Author: Henri Keeble
	Desc: Declares a class used to store an entire track as a collection of segments in a doubly linked list structure. Segments can be retrieved from here
		  through the () operator overload to add back into the TORCS track structure. This prevents TORCS from generating 3D descriptions that are too large.
*/

#ifndef _P_SEG_COLLECTION_H_
#define _P_SEG_COLLECTION_H_

#include "track.h"

namespace procedural
{
	class pSegCollection
	{
	private:
		tTrackSeg* end;
		tTrackSeg* start;
		int nseg;
		int length;
		
		void cpy(const pSegCollection& param); // Internal copying function
	public:
		pSegCollection();
		pSegCollection(const pSegCollection& param);
		~pSegCollection();
		
		// Operator Overloads
		pSegCollection& operator=(const pSegCollection& param);
		tTrackSeg* operator()(int id);
		
		// Get Accessors
		tTrackSeg* Start();
		tTrackSeg* End();
		int nSegs() const;
		int Length() const;
		
		void AddSegment(tTrackSeg* seg);
	};
}

#endif // _P_SEG_COLLECTION_H_