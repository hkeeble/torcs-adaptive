/*
	File: PRange.h
	Author: Henri Keeble
	Desc: Contains declarations for a class and collection of specified maximum and minimum ranges for track parameters.
*/
#ifndef _P_RANGE_H_
#define _P_RANGE_H_

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
		PRange arc, radius;
	public:
		PSegmentRanges(PRange arc, PRange radius);
		~PSegmentRanges();
		
		PRange Arc() 	const;
		PRange Radius()  const;
	};
}

#endif // _P_RANGE_H_