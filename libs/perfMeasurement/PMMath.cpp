/*
	File: PMMath.cpp
	Author: Henri Keeble
	Desc: Definitions for maths structures and functions.
*/

#include "perfMeasurement\PMMath.h"

namespace perfMeasurement
{
	typedef float tdble;

	PMLineSeg PMMath::CalculatePerpendicularBisector(PMLineSeg line)
	{
		PMLineSeg perpLine = PMLineSeg();
		PMPoint2D midPoint = PMPoint2D();

		// Find midpoint
		midPoint.x = (line.end.x - line.start.x) / 2;
		midPoint.y = (line.end.y - line.start.y) / 2;

		// Calculate perpendicular bisector
		tdble slopeNR = -(1 / line.slope);

		// Slope of new line is the negative reciprocal of the the slope of the original
		perpLine.slope = slopeNR;
		perpLine.intercept = -(perpLine.slope*midPoint.x - midPoint.y);

		return perpLine;
	}
		
	tdble PMMath::DistBetweenPoints(PMPoint2D a, PMPoint2D b)
	{
		tdble distx = a.x - b.x;
		tdble disty = a.y - b.y;
		if (distx < 0)
			distx *= -1;
		if (disty < 0)
			disty *= -1;
		return distx + disty;
	}

	tdble PMMath::CalculateArcRadius(PMPoint2D* points, int nOfPoints)
	{
		tdble rad = 0.f;

		if (points != nullptr)
		{
			// Get line segments between optimal points
			PMLineSeg* lineSegs = new PMLineSeg[nOfPoints - 1];
			for (int i = 0; i < nOfPoints - 1; i++)
				lineSegs[i] = PMLineSeg(points[i], points[i + 1]);

			// Find the perpendicular bisector of the lines
			PMLineSeg* bisectingLines = new PMLineSeg[nOfPoints - 1];
			for (int i = 0; i < nOfPoints - 1; i++)
				bisectingLines[i] = CalculatePerpendicularBisector(lineSegs[i]);

			// Turn line segments into lines
			PMLine* lines = new PMLine[nOfPoints - 1];
			for (int i = 0; i < nOfPoints - 1; i++)
				lines[i] = bisectingLines[i];

			// Find intersection point of the lines
			if (lines[0].Intersects(lines[1]))
			{
				PMPoint2D intersectionPoint = lines[0].IntersectionPoint(lines[1]);
				rad = DistBetweenPoints(intersectionPoint, *points);
			}
			else
			{
				pmOut("WARNING: Unable to find intersection point between perpendicular bisectors of the optimal path arc chords.\n");
				rad = 0.f;
			}

			// Delete unused data
			delete[] lines;
			delete[] bisectingLines;
			delete[] lineSegs;

			return rad;
		}
		else
		{
			pmOut("Unable to calculate optimal radius, returning 0.f.\n");
			return 0.f;
		}
	}

	PMPoint2D::PMPoint2D()
	{
		x = y = 0;
	}

	PMLine::PMLine()
	{
		slope = 0.f;
		intercept = 0.f;
	}

		
	PMPoint2D PMLine::IntersectionPoint(PMLine otherLine)
	{
		if (Intersects(otherLine))
			return PMPoint2D();
		else
		{
			PMPoint2D point = PMPoint2D();
			point.x = (otherLine.intercept - intercept) / (slope - otherLine.slope);
			point.y = slope * point.x + intercept;
			return point;
		}
	}
		
	bool PMLine::Intersects(PMLine otherLine)
	{
		if (slope - otherLine.slope == 0)
			return false;
		else
			return true;
	}
		
	PMLine& PMLine::operator=(const PMLineSeg& param)
	{
		if(this == &param)
			return *this;
		else
		{
			slope = param.slope;
			intercept = param.intercept;
			return *this;
		}
	}
		
	PMLine& PMLine::operator=(const PMLine& param)
	{
		if(this == &param)
			return *this;
		else
		{
			slope = param.slope;
			intercept = param.intercept;
			return *this;
		}
	}
		
	PMLineSeg::PMLineSeg()
	{
		start = PMPoint2D();
		end = PMPoint2D();
		slope = 0.f;
		intercept = 0.f;
	}
		
	PMLineSeg::PMLineSeg(PMPoint2D Start, PMPoint2D End)
	{
		start = Start;
		end = End;
		slope = (end.y - start.y) / (end.x - start.x);
		intercept = end.y - (slope*end.x);
	}
}