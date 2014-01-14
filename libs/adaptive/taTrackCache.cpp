/*
	File: taTrackCache.cpp
	Author: Henri Keeble
	Desc: Defines a class to be used to cache a track's segments.
*/
#include "torcsAdaptive\taTrackCache.h"

namespace torcsAdaptive
{
	taTrackCache::taTrackCache()
	{
		end = nullptr;
		start = nullptr;
		nseg = 0;
		length = 0;
	}

	taTrackCache::taTrackCache(const taTrackCache& param)
	{
		cpy(param);
	}

	taTrackCache::~taTrackCache()
	{
		if (start)
			delete start;
		if (end)
			delete end;
	}

	taTrackCache& taTrackCache::operator=(const taTrackCache& param)
	{
		if (this == &param)
			return *this;
		else
		{
			if (start)
				delete start;
			if (end)
				delete end;

			cpy(param);
			return *this;
		}
	}

	tTrackSeg* taTrackCache::operator()(int id)
	{
		tTrackSeg* curSeg = start;
		do {
			curSeg = curSeg->next;
		} while (curSeg->id != id || curSeg == start);

		if (curSeg->id != id)
			return nullptr;

		return curSeg;
	}

	tTrackSeg* taTrackCache::Start()
	{
		return start;
	}

	tTrackSeg* taTrackCache::End()
	{
		return end;
	}

	int taTrackCache::nSegs() const
	{
		return nseg;
	}

	int taTrackCache::Length() const
	{
		return length;
	}

	void taTrackCache::AddSegment(tTrackSeg* seg)
	{
		// Copy the segment passed in
		tTrackSeg* newSeg = new tTrackSeg();
		*newSeg = *seg;

		if (nseg == 0)
		{
			start = newSeg;
			end = newSeg;
			start->next = end;
			start->prev = end;
			end->next = start;
			end->prev = start;
		}
		else
		{
			end->next = newSeg;
			newSeg->prev = end;
			newSeg->next = start;
			end = newSeg;
		}

		nseg++;
		length += end->length;
	}

	void taTrackCache::cpy(const taTrackCache& param) // Internal copying function
	{
		taTrackCache& p = const_cast<taTrackCache&>(param); // Cast away constness, for use in AddSegment

		if (p.nseg > 0)
		{
			for (int i = 0; i < p.nseg; i++)
				AddSegment(p(i));
		}
		else
		{
			start = nullptr;
			end = nullptr;
		}
	}
}