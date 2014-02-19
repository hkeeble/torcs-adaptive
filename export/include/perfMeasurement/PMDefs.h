/*
	File: PMDefs.h
	Author: Henri Keeble
	Desc: Definitions for some commonly used macros in the PM library.
*/

namespace perfMeasurement
{
	#define PERFMEASURE_UPDATE_INTERVAL 0.1

	#define NULL_SKILL_LEVEL -1.f

	#ifdef _DEBUG
		#define pmOut(out) std::cout << "perfMeasurement >>" + std::string(out)
	#else
		#define pmOut(out)
	#endif // _DEBUG
}