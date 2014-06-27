#include "PPathfinder.h"

#ifndef _PATH_PLANNER_H_
#define _PATH_PLANNER_H_

namespace procPathfinder
{
	/* Forward declares? */
	class PCarDesc;
	class POtherCarDesc;
	class PPathfinder;

	/* holds data relative to my car */
	typedef struct {
		double speedsqr;		/* on track direction projected speed squared of opponent */
		double speed;			/* same, but not squared */
		double time;			/* estimate of time to catch up the car */
		double cosalpha;		/* cos(alpha) from angle between my ond opponent */
		double disttomiddle;	/* distance to middle (for prediction) */
		int catchdist;
		int catchsegid;			/* segment, where i expect (or better guess!) to catch opponent */
		double dist;			/* #segments from me to the other car */
		POtherCarDesc* collcar;		/* pointers to the cars */
		bool overtakee;			/* is this the guy to overtake? */
		double disttopath;		/* distance to my path */
		double brakedist;		/* distance needed for braking to the speed of this car */
		double mincorner;		/* corner nearest to my car */
		double minorthdist;		/* minimal distance relative to my car */
	} tOCar;

	/* holds data needed for let pass opponents */
	typedef struct {
		double time;			/* how long is the opponent "in range" to overlap me */
	} tOverlapTimer;

	class PPathPlanner
	{
	public:
		PPathPlanner(PPathfinder* pathfinder, tSituation* s);
		~PPathPlanner();

		void Plan(int trackSegId, tCarElt* car, tSituation* situation, PCarDesc* myc, POtherCarDesc* ocar);

		PPathfinder* path;	/* Pathfinding algorithm used by this path planner. */

	private:
		int lastPlan;				/* start of the last plan */
		int lastPlanRange;			/* range of the last plan */
		int collcars;
		int changed;

		tOCar* o;
		tOverlapTimer* overlaptimer;

		static const double COLLDIST;	/* up to this distance do we consider other cars as dangerous */

		void Init(tSituation* s);

		void Smooth(int s, int e, int p, double w);

		int Collision(int trackSegId, tCarElt* mycar, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar);
		int Overtake(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar);
		int CorrectPath(int id, tCarElt* car, PCarDesc* myc);
		double PathSlope(int id);
		int Letoverlap(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOverlapTimer* ov);
		int CountSegments(int from, int to);
		int UpdateOCar(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o);
		void UpdateOverlapTimer(int trackSegId, tSituation *s, PCarDesc* myc, POtherCarDesc* ocar, tOCar* o, tOverlapTimer* ov);
	};
}

#endif // _PATH_PLANNER_H_