/*
	File: pSeg.h
	Author: Henri Keeble
	Desc: Declares a class used to represent a procedural segment, and factory functions to ease instantiation of different types of segment.
*/

#ifndef _P_SEG_H_
#define _P_SEG_H_

#include <string>

namespace procedural
{
	class pSegFactory;

	/* Represents segment data for torcs-adaptive. */
	class pSeg
	{
	private:
		pSeg();
	public:	
		friend class pSegFactory;

		virtual ~pSeg();

		int	id;	
		int type;
		int type2;
		int style;
		int raceInfo;

		// Data
		float length;
				
		// For Corners
		float radius;
		float radiusr;
		float radiusl;
		float arc;	
	};

	enum pCornerType
	{
		TaRight = 1,
		TaLeft = 2
	};

	/* Factory for segments, singleton class */
	class pSegFactory
	{
	private:
		pSegFactory();
		virtual ~pSegFactory();
		static pSegFactory* instance;
	public:
		static pSegFactory* GetInstance();

		// Creates a straight segment
		pSeg CreateSegStr(int id, float length);

		// Creates a corner segment
		pSeg CreateSegCnr(int id, pCornerType cType, float radius, float radiusr, float radiusl, float arc);
	};
}
#endif // _P_SEG_H_