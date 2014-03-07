/*
	File: PSeg.cpp
	Author: Henri Keeble
	Desc: Defines a class used to represent a procedural segment, and factory functions to ease instantiation of different types of segment.
*/
#include "PSeg.h"

namespace procedural
{
	PSeg::PSeg()
	{
		id = type = 0;
		length = radius = arc = 0.f;

		raceInfo = 0x00000000; // Normal Segment
		type2 = 1; // Main Track Segment
		style = 2; // Plan (TR_PLAN)
	}

	PSeg::~PSeg()
	{

	}

	PSegFactory* PSegFactory::instance = NULL;

	PSegFactory::PSegFactory() : ranges(PSegmentRanges(PRange(MIN_ARC, MAX_ARC), PRange(MIN_RADIUS, MAX_RADIUS), PRange(MIN_LENGTH, MAX_LENGTH)))
	{
		cornerChance = straightChance = 50.f;
		srand(time(0));
		previousCornerType = (RandBetween(0, 100) > 50 ? PCornerType::CTLeft : PCornerType::CTRight);
	}

	PSegFactory::~PSegFactory()
	{
		if(instance)
			delete instance;
	}

	PSegFactory* PSegFactory::GetInstance()
	{
		if(instance == NULL)
			instance = new PSegFactory();
		return instance;
	}

	PSeg PSegFactory::CreateSegStr(int id, float length)
	{
		PSeg newSeg = PSeg(); // Default Constructor takes care of shared default values

		newSeg.type = 3; // TR_STR #define
		newSeg.id = id;
		newSeg.length = length;

		return newSeg; // Return a segment that has been ratified
	}

	PSeg PSegFactory::CreateSegCnr(int id, PCornerType cType, float radius, float arc)
	{
		PSeg newSeg = PSeg();

		newSeg.id = id;
		newSeg.type = cType;
		newSeg.radius = radius;
		newSeg.arc = arc;

		return newSeg;  // Return a segment that has been ratified
	}

	PSeg PSegFactory::CreateRandomSeg(int id)
	{
		PSegType type = RandomSegType();

		if (type == PSegType::Straight)
			return CreateRandomStr(id);
		else
			return CreateRandomCnr(id);
	}

	PSeg PSegFactory::CreateRandomStr(int id)
	{
		float length	= RandBetween(ranges.Length().Min(), ranges.Length().Max());

		return CreateSegStr(id, length);
	}

	PSeg PSegFactory::CreateRandomCnr(int id)
	{
		// Generate segment parameters
		float radius = RandBetween(ranges.Radius().Min(),	ranges.Radius().Max());
		float arc	 = RandBetween(ranges.Arc().Min(),		ranges.Arc().Max());

		PCornerType cType;
		
		// Decide upon corner type
		if (previousCornerType == CTLeft)
			cType = CTRight;
		else
			cType = CTLeft;

		previousCornerType = cType;

		// Return the new segment
		return CreateSegCnr(id, cType, radius, arc);
	}

	void PSegFactory::SetChances(float corner, float straight)
	{
		if (corner + straight == 100) // Chanes must equal 100, or they are defaulted to 50/50
		{
			cornerChance = corner;
			straightChance = straight;
		}
		else
		{
			pOut("PSegFactory: Error! Corner and straight segment generation chances must equal 100!");
			cornerChance = straightChance = 50.f;
		}
	}

	float PSegFactory::RandBetween(float min, float max)
	{
		if (min < max)
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min))); // Static cast of rand() produces a floating point value
		else
			pOut("PSegFactory: Min cannot be less thatn Max!\n");
	}

	PSegType PSegFactory::RandomSegType()
	{
		PSegType type;

		// Determine highest chance value
		int highVal = PSegType::Straight;
		if (straightChance < cornerChance)
			highVal = PSegType::Corner;

		float val = rand() % 100; // Generate random value

		if (highVal == PSegType::Corner)
		{
			if (val >= cornerChance)
				type = PSegType::Corner;
			else
				type = PSegType::Straight;
		}
		else if (highVal == PSegType::Straight)
		{
			if (val >= straightChance)
				type = PSegType::Straight;
			else
				type = PSegType::Corner;
		}

		return type;
	}
}