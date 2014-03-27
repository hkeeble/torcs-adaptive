/*
	File: PRange.h
	Author: Henri Keeble
	Desc: Contains declarations for a class and collection of specified maximum and minimum ranges for track parameters.
*/
#ifndef _P_RANGE_H_
#define _P_RANGE_H_

#include "tgf.h"

namespace procedural
{
	struct PRange
	{
	private:
		float min, max;
	public:
		PRange();
		PRange(float Min, float Max);
		float Min() const;
		float Max() const;
	};
	
	class PSegmentRanges
	{
	private:
		PRange larc, rarc, radius, length;
	public:
		PSegmentRanges(PRange angle, PRange radius, PRange length);
		~PSegmentRanges();
		
		PRange LeftArc()  const;
		PRange RightArc() const;
		PRange Radius()   const;
		PRange Length()   const;

		void SetLeftArc(const PRange& range);
		void SetRightArc(const PRange& range);
		void SetRadius(const PRange& range);
		void SetLength(const PRange& range);
	};
}

#endif // _P_RANGE_H_