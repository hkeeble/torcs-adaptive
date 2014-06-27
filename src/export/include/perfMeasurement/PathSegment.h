/**
 *	@file PathSegment.h
 *	@author Henri Keeble
 *	@brief Declares a class that represents an individual point on a calculated path.
*/

#ifndef _PATH_SEGMENT_H_
#define _PATH_SEGMENT_H_

#include "taMath\taMath.h"

namespace perfMeasurement
{
	class PathSegment
	{
	public:
		PathSegment(const v2d& newLoc) : loc(newLoc) { }
		~PathSegment() { };
		
		/*!< Get the location of this path segment. */
		inline v2d getLoc() { return loc; };
		
		/*!< Set the location of this path segment. */
		inline void setLoc(const v2d& newLoc) { loc = newLoc; };
		
	private:
		v2d loc; /*!< Location of this path segment. */
	};
}

#endif // _PATH_SEGMENT_H_