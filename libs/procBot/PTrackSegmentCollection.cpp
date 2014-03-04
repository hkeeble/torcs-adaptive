/*
	File: PTrackSegment.h
	Author: Henri Keeble
	Desc: Defines a class used to store a collection of approximtely 1 meter long track segments.
*/

#include "PTrackSegmentCollection.h"

namespace procBot
{
	PTrackSegmentCollection::PTrackSegmentCollection(PTrackSegment* segs, int numberOfSegs)
	{
		this->segs = segs;
		nSegs = numberOfSegs;
	}

	void PTrackSegmentCollection::cpy(const PTrackSegmentCollection& param)
	{
		if (param.segs)
		{
			this->segs = new PTrackSegment[param.Length()];
			nSegs = param.Length();
			for (int i = 0; i < nSegs; i++)
				this->segs[i] = param.segs[i];
		}
	}

	PTrackSegmentCollection::PTrackSegmentCollection(const PTrackSegmentCollection& param)
	{
		cpy(param);
	}

	PTrackSegmentCollection& PTrackSegmentCollection::operator=(const PTrackSegmentCollection& param)
	{
		if (this == &param)
			return *this;
		else
		{
			if (segs)
				delete[] segs;
			cpy(param);
			return *this;
		}
	}

	PTrackSegmentCollection::~PTrackSegmentCollection()
	{
		if (segs)
			delete[] segs;
	}

	PTrackSegment* PTrackSegmentCollection::operator()(int index)
	{
		if (index < nSegs)
			return &segs[index];
		else
			return nullptr;
	}

	int PTrackSegmentCollection::Length() const
	{
		return nSegs;
	}

	void PTrackSegmentCollection::Append(PTrackSegment* newSegs, int nOfNewSegs)
	{
		PTrackSegment* tmp = new PTrackSegment[nSegs + nOfNewSegs];

		// Copy old segments into temporary array
		for (int i = 0; i < nSegs; i++)
			tmp[i] = segs[i];

		// Copy new segments into temporary array
		for (int i = nSegs; i < nSegs + nOfNewSegs; i++)
			tmp[i] = newSegs[i];

		// Assign temporary array to segs
		segs = tmp;

		// Calculate new number of segments
		nSegs = nSegs + nOfNewSegs;

		for (int i = 0; i < nSegs; i++)
			std::cout << segs[i].getWidth() << std::endl;
	}
}