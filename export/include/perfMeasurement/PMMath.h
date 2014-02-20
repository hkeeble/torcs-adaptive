/*
	File: PMMath.h
	Author: Henri Keeble
	Desc: Declarations for maths structures and functions.
*/

#ifndef _PM_MATH_H_
#define _PM_MATH_H_

#include "PMDefs.h"

namespace perfMeasurement
{
	// Forward declarations
	struct PMPoint2D;
	struct PMLine;
	struct PMLineSeg;
	class PMMath;

	struct PMPoint2D
	{
		PMPoint2D();

		tdble x;
		tdble y;
	};

	struct PMLine
	{
		PMLine();

		PMPoint2D IntersectionPoint(PMLine otherLine);
		bool Intersects(PMLine otherLine);
		PMLine& operator=(const PMLineSeg& param);
		PMLine& operator=(const PMLine& param);

		tdble slope;
		tdble intercept;
	};

	struct PMLineSeg : public PMLine
	{
		PMLineSeg();

		PMLineSeg(PMPoint2D Start, PMPoint2D End);

		PMPoint2D start;
		PMPoint2D end;
	};

	class PMMath
	{
	private:
		PMMath() { }
		PMMath(const PMMath& param) { }
		PMMath& operator=(const PMMath& param) { };
	public:
		/* Used to calculate a perpendicular bisector of the line passed in */
		static PMLineSeg CalculatePerpendicularBisector(PMLineSeg line);

		/* Used to calculate the distance between two given points*/
		static tdble DistBetweenPoints(PMPoint2D a, PMPoint2D b);
	};
}

#endif // _PM_MATH_H_