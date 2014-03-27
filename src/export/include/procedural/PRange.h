/**
	@file PRange.h
	@author Henri Keeble
	@brief Contains declarations for a class and collection of specified maximum and minimum ranges for track parameters.
*/
#ifndef _P_RANGE_H_
#define _P_RANGE_H_

#include "tgf.h"

namespace procedural
{
	//! Represents a range between a minimum and maximum floating point value.
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
	
	//! A class containing all the ranges for segment parameter generation.
	class PSegmentRanges
	{
	private:
		PRange larc, rarc, radius, length;
	public:
		//! Create a new set of ranges with the given parameters. 
		PSegmentRanges(PRange angle, PRange radius, PRange length);
		~PSegmentRanges();
		
		PRange LeftArc()  const; /*!< Get the current left arc range. */
		PRange RightArc() const; /*!< Get the current right arc range. */
		PRange Radius()   const; /*!< Get the current radius range. */
		PRange Length()   const; /*!< Get the current length range. */

		void SetLeftArc(const PRange& range); /*!< Set the current left arc range. */
		void SetRightArc(const PRange& range); /*!< Set the current right arc range. */
		void SetRadius(const PRange& range); /*!< Set the current radius range. */
		void SetLength(const PRange& range); /*!< Set the current length range. */
	};
}

#endif // _P_RANGE_H_