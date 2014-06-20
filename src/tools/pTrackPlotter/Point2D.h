#ifndef _VECTOR_2D_
#define _VECTOR_2D_

struct Point2D
{
	double x, y;

	Point2D() { x = y = 0; }

	Point2D(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
};

#endif // _VECTOR_2D_