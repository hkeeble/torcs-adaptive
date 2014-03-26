/*
	File: PRange.h
	Author: Henri Keeble
	Desc: Contains a class and collection for a specified maximum and minimum range for track parameters.
*/
#include "PRange.h"

namespace procedural
{
	PRange::PRange()
	{
		min = max = 0;
	}

	PRange::PRange(float Min, float Max)
	{
		min = Min;
		max = Max;
	}

	float PRange::Min() const
	{
		return min;
	}

	float PRange::Max() const
	{
		return max;
	}

	PSegmentRanges::PSegmentRanges(PRange arc, PRange radius, PRange length)
	{
		this->arc = arc;
		this->radius = radius;
		this->length = length;
	}

	PSegmentRanges::~PSegmentRanges()
	{

	}

	PRange PSegmentRanges::Arc() const
	{
		return arc;
	}

	PRange PSegmentRanges::Radius() const
	{
		return radius;
	}

	PRange PSegmentRanges::Length() const
	{
		return length;
	}

	void PSegmentRanges::SetArc(const PRange& range)
	{
		arc = range;
	}

	void PSegmentRanges::SetRadius(const PRange& range)
	{
		radius = range;
	}

	void PSegmentRanges::SetLength(const PRange& range)
	{
		length = range;
	}
}