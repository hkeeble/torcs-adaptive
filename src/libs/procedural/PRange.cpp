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
		this->radius = radius;
		this->length = length;
		this->larc = arc;
		this->rarc = arc;
	}

	PSegmentRanges::~PSegmentRanges()
	{

	}

	PRange PSegmentRanges::LeftArc() const
	{
		return larc;
	}

	PRange PSegmentRanges::RightArc() const
	{
		return rarc;
	}

	PRange PSegmentRanges::Radius() const
	{
		return radius;
	}

	PRange PSegmentRanges::Length() const
	{
		return length;
	}

	void PSegmentRanges::SetLeftArc(const PRange& range)
	{
		larc = range;
	}

	void PSegmentRanges::SetRightArc(const PRange& range)
	{
		rarc = range;
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