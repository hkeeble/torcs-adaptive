/*
	File: PSegCollection.cpp
	Author: Henri Keeble
	Desc: Defines a class used to store an entire track as a collection of segments in a doubly linked list structure. Segments can be retrieved from here
	      through the () operator overload to add back into the TORCS track structure. This prevents TORCS from generating 3D descriptions that are too large.
*/
#include "PSegCollection.h"

namespace procedural
{
	PSegCollection::PSegCollection()
	{
		end = nullptr;
		start = nullptr;
		nseg = 0;
		length = 0;
	}

	PSegCollection::PSegCollection(const PSegCollection& param)
	{
		cpy(param);
	}

	PSegCollection::~PSegCollection()
	{
		if (start)
			delete start;
		if (end)
			delete end;
	}

	PSegCollection& PSegCollection::operator=(const PSegCollection& param)
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

	tTrackSeg* PSegCollection::operator()(int id)
	{
		tTrackSeg* curSeg = start;

		// Loop until segment is found, or entire loop of track is made
		do {
			curSeg = curSeg->next;
		} while (curSeg->id != id && curSeg != start);

		// If seg not found, return null
		if (curSeg->id != id)
			return nullptr;

		return curSeg;
	}

	tTrackSeg* PSegCollection::Start()
	{
		return start;
	}

	tTrackSeg* PSegCollection::End()
	{
		return end;
	}

	int PSegCollection::nSegs() const
	{
		return nseg;
	}

	int PSegCollection::Length() const
	{
		return length;
	}

	void PSegCollection::AddSegment(tTrackSeg* seg)
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

	void PSegCollection::cpy(const PSegCollection& param) // Internal copying function
	{
		PSegCollection& p = const_cast<PSegCollection&>(param); // Cast away constness, for use in AddSegment

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