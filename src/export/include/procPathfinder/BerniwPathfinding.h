#include "PPathfinder.h"

#ifndef _BERNIW_PATHFINDING_H_
#define _BERNIW_PATHFINDING_H_

namespace procPathfinder
{
	/* holds a point of datafile from clothoid */
	typedef struct {
		double x;
		double pd;
		double is;
		double ic;
	} tParam;

	class BerniwPathfinding : public PPathfinder
	{
	public:
		BerniwPathfinding(tTrack* itrack, tCarElt* icar);
		~BerniwPathfinding();

		/*!< Plan the path. Override in base classes. */
		void Plan(PCarDesc* myc) override final;

	private:
		tParam cp[NTPARAMS]; /* holds values needed for clothiod */

		bool loadClothoidParams(tParam* p);
		double intsinsqr(double alpha);
		double intcossqr(double alpha);
		double clothparam(double alpha);
		double clothsigma(double beta, double y);
		double clothlength(double beta, double y);

		int findStartSegId(int id);
		int findEndSegId(int id);

		int initStraight(int id, double w);
		int initLeft(int id, double w);
		int initRight(int id, double w);

		double computeWeight(double x, double len);
		void setLocWeighted(int id, double newweight, v3d* newp);

		void smooth(int s, int e, int p, double w);
		void smooth(int id, double delta, double w);
		void optimize(int start, int range, double w);
		void optimize2(int start, int range, double w);
		void optimize3(int start, int range, double w);
	};
}

#endif // _BERNIW_PATHFINDING_H_