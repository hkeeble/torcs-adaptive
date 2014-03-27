/**
 *  @file PDefs.h
 *	@author Henri Keeble
 *	@brief Some preprocessor defines and other definitions common to many procedural translation units.
*/

#ifndef _P_DEFS_
#define _P_DEFS_

#include "../../../windows/include/plib/ssg.h"

namespace procedural
{
	///! Alias for SSG entity.
	typedef ssgEntity EntityDesc;

	// Maximum and minimum ranges
	#define MIN_LENGTH 50
	#define MAX_LENGTH 150

	#define MAX_ARC 1.5f
	#define MIN_ARC 0.1f

	#define MAX_RADIUS 90.f
	#define MIN_RADIUS 25.f

#ifdef _DEBUG
	#define pOut(out) std::cout << "procedural >>" + std::string(out)
#else
	#define pOut(out)
#endif // _DEBUG

	//! Represents segment type within the procedural library.
	enum PSegType
	{
		Straight = 0, /*!< A straight segment. */
		Corner = 1 	  /*!< A corner segment. */
	};
}
#endif _P_DEFS_