/*
	File: taMath.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Maths functions exported by Henri Keeble for TORCS Adaptive - also could make rest of TORCS code far more modular.
*/

#include "linalg.h"
#include "spline.h"
#include <float.h>

#ifndef _TA_MATH_H_
#define _TA_MATH_H_

struct Rect
{
	Rect() { width = height = x = y = 0; }
	Rect(int w, int h, int x, int y) { width = w; height = h; this->x = x; this->y = y; }

	int width;
	int height;
	int x;
	int y;
};

inline double sign(double number)
{
	return (number >= 0.0) ? 1.0 : -1.0;
}


inline double sqr(double x)
{
	return x*x;
}

inline double lerp(double x, double y, double coefficient)
{
	return x + coefficient * (y - x);
}

/* compute the radius given three points */
inline double radius(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double dx1 = x2 - x1;
	double dy1 = y2 - y1;
	double dx2 = x3 - x2;
	double dy2 = y3 - y2;

	//double z = (dy1*dx2 - dx1*dy2);
	double z = dx1*dy2 - dy1*dx2;

	if (z != 0.0) {
		double k = (dx2*(x3-x1) - dy2*(y1-y3))/z;
		return sign(z)*sqrt((1.0+k*k)*(dx1*dx1+dy1*dy1))/2.0;
	} else {
		return FLT_MAX;
	}
}

#endif // _TA_MATH_H_