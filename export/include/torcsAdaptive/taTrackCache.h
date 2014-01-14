/*
	File: taTrackCache.h
	Author: Henri Keeble
	Desc: Declares a class to be used to cache a track's segments.
*/

#ifndef _TRACK_CACHE_H_
#define _TRACK_CACHE_H_

#include "track.h"

namespace torcsAdaptive
{
	class taTrackCache
	{
	private:
		tTrackSeg* end;
		tTrackSeg* start;
		int nseg;
		int length;
		
		void cpy(const taTrackCache& param); // Internal copying function
	public:
		taTrackCache();
		taTrackCache(const taTrackCache& param);
		~taTrackCache();
		
		// Operator Overloads
		taTrackCache& operator=(const taTrackCache& param);
		tTrackSeg* operator()(int id);
		
		// Get Accessors
		tTrackSeg* Start();
		tTrackSeg* End();
		int nSegs() const;
		int Length() const;
		
		void AddSegment(tTrackSeg* seg);
	};
}

#endif // _TRACK_CACHE_H_