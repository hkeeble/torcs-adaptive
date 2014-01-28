/*
	File: Spline.h
	Author: Bernhard Wymann, Henri Keeble (edits)
	Desc:Declarations for Spline and related classes and structs. This is a modified version of the original berniw robot for TORCS Adaptive.
*/

#ifndef _SPLINE_H_
#define _SPLINE_H_

// Structs to arrange data cache-friendly.
typedef struct {
	double a, b, c, d, h;
} SplineEquationData;

typedef struct {
	double a, b, c, d, h, x1, x2;
} SplineEquationData2;


/* solves tridiagonal matrix in linear time O(n) with Given's Rotations */
extern void tridiagonal(int dim, SplineEquationData *tmp, double *x);


/* solving tridiagonal nxn matrix for two vectors with Givens-Rotations in linear time O(n) */
extern void tridiagonal2(int dim, SplineEquationData2 *tmp);


/* compute the slopes of the spline points with periodic constraints */
extern void slopesp(int dim, const double *const x, const double *const y, double *const ys);


/* compute the slopes of the spline points with natural constraints */
extern void slopesn(int dim, const double *const x, const double *const y, double *const ys);


/* compute the slopes for 2-dim curve, sums euclidian distances as parameter, periodic */
extern void parametricslopesp(
	int dim,
	const double *const x,
	const double *const y,
	double *const xs,
	double *const ys,
	double *const s
);


/* compute the slopes for 2-dim curve, sums euclidian distances as parameter, natural */
extern void parametricslopesn(
	int dim,
	const double *const x,
	const double *const y,
	double *const xs,
	double *const ys,
	double *const s
);


/* compute the y value for a given z */
extern double spline(
	int dim,
	double z,
	const double *const x,
	const double *const y,
	const double *const ys
);


#endif // _SPLINE_H_

