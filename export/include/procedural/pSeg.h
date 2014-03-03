/*
	File: PSeg.h
	Author: Henri Keeble
	Desc: Declares a class used to represent a procedural segment, and factory functions to ease instantiation of different types of segment.
*/

#ifndef _P_SEG_H_
#define _P_SEG_H_

#include <string>
#include <iostream>
#include <ctime>
#include "PRange.h"
#include "PDefs.h"

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
		float radius;
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

		float cornerChance;
		float straightChance;

		/* Generate random floating point value between min and max */
		float RandBetween(float min, float max);

		/* Generate a random segment type based on the chances currently set */
		PSegType RandomSegType();

		/* The previously used corner type */
		PCornerType previousCornerType;
	public:
		static PSegFactory* GetInstance();

		/* The arbitrary ranges between which segment parameters may reside */
		const PSegmentRanges ranges = PSegmentRanges(PRange(MIN_ARC, MAX_ARC), PRange(MIN_RADIUS, MAX_RADIUS), PRange(MIN_LENGTH, MAX_LENGTH));
		
		/* Sets the percentage chances or corners and straights being generated */
		void SetChances(float corner, float straight);

		/* Generates a random segment based upon chances and ranges set */
		PSeg CreateRandomSeg(int id);

		/* Generates a random straight segment */
		PSeg CreateRandomStr(int id);

		/* Generates a random corner segment */
		PSeg CreateRandomCnr(int id);

		/* Creates a straight segment */
		PSeg CreateSegStr(int id, float length);

		/* Creates a corner segment */
		PSeg CreateSegCnr(int id, PCornerType cType, float radius, float arc);
	};
}
#endif // _P_SEG_H_