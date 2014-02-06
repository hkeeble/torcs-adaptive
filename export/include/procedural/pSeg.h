/*
	File: PSeg.h
	Author: Henri Keeble
	Desc: Declares a class used to represent a procedural segment, and factory functions to ease instantiation of different types of segment.
*/

#ifndef _P_SEG_H_
#define _P_SEG_H_

#include <string>

namespace procedural
{
	class PSegFactory;

	/* Represents segment data for torcs-adaptive. */
	class PSeg
	{
	private:
		PSeg();
	public:	
		friend class PSegFactory;

		virtual ~PSeg();

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

	enum PCornerType
	{
		CTRight = 1,
		CTLeft = 2
	};

	/* Factory for segments, singleton class */
	class PSegFactory
	{
	private:
		PSegFactory();
		virtual ~PSegFactory();
		static PSegFactory* instance;
	public:
		static PSegFactory* GetInstance();

		// Creates a straight segment
		PSeg CreateSegStr(int id, float length);

		// Creates a corner segment
		PSeg CreateSegCnr(int id, PCornerType cType, float radius, float radiusr, float radiusl, float arc);
	};
}
#endif // _P_SEG_H_