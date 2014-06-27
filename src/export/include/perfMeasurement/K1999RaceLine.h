/**
 *	@file K1999RaceLine.h
 *	@author Henri Keeble
 *	@brief Declares a pathfinder class that uses K1999 algorithm by Remi Coulom.
*/

#ifndef _K1999_RACE_LINE_H_
#define _K1999_RACE_LINE_H_

#include "Pathfinder.h"

namespace perfMeasurement
{
	class K1999RaceLine : public Pathfinder
	{
	public:
		K1999RaceLine(tTrack* theTrack);
		~K1999RaceLine();
		
		virtual void Plan() override final;
		virtual void UpdatePlan() override final;
	};
}

#endif // _K1999_RACE_LINE_H_