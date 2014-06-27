/**
 *	@file Pathfinder.h
 *	@author Henri Keeble
 *	@brief Declares a base class to use for pathfinders used in race line evaluation.
*/

#ifndef _PATHFINDER_H_
#define _PATHFINDER_H_

#include <vector>
#include <string>
#include <fstream>

#include "PathSegment.h"
#include "track.h"

#define OPTIMAL_PLOT "optimal.dat"
#define ACTUAL_PLOT "actual.dat"

namespace perfMeasurement
{
	class Pathfinder
	{
	public:
		Pathfinder(tTrack* track) : segments(std::vector<PathSegment>()), theTrack(track), pointsOnLastOutput(0) { };
		~Pathfinder() { };
		
		virtual void Plan() = 0;
		virtual void UpdatePlan() = 0;
		
		PathSegment Seg(int i) { return segments[i]; }
		
		void Output(std::string string);
		
	protected:
		std::vector<PathSegment> segments; /*!< Individual segments contained within this path. */
		tTrack* theTrack; /*!< Track object to which the pathfinder is attached. */
	private:
		int pointsOnLastOutput;
	};
}

#endif // _PATHFINDER_H_