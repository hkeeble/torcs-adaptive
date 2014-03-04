/*
	File: PTrackSegment.h
	Author: Henri Keeble
	Desc: Defines a class used to store a collection of approximtely 1 meter long track segments.
*/

#include "PTrackSegmentCollection.h"

namespace procBot
{
	PTrackSegmentCollection::PTrackSegmentCollection(const vector<PTrackSegment>& segs)
	{
		this->segs = segs;
	}

	void PTrackSegmentCollection::cpy(const PTrackSegmentCollection& param)
	{
		this->segs = param.segs;
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
			cpy(param);
			return *this;
		}
	}

	PTrackSegmentCollection::~PTrackSegmentCollection()
	{
		// nothing yet
	}

	PTrackSegment* PTrackSegmentCollection::operator()(int index)
	{
		if (index < segs.size())
			return &segs[index];
		else
			return nullptr;
	}

	int PTrackSegmentCollection::Count() const
	{
		return segs.size();
	}

	void PTrackSegmentCollection::Append(const vector<PTrackSegment>& newSegs)
	{
		for (auto i : newSegs) // range based for, simply appends to the existing vector with data from the new one
			segs.push_back(i);

		std::cout << "New segment collection: " << std::endl;
		for (int i = 0; i < segs.size(); i++)
			std::cout << segs[i].getTrackSegment()->id << std::endl;
	}
}