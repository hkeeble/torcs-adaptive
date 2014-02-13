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
		length = radius = radiusr = radiusl = arc = 0.f;

		raceInfo = 0x00000000; // Normal Segment
		type2 = 1; // Main Track Segment
		style = 2; // Plan (TR_PLAN)
	}

	PSeg::~PSeg()
	{

	}

	PSegFactory* PSegFactory::instance = NULL;

	PSegFactory::PSegFactory()
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

		return newSeg;
	}

	PSeg PSegFactory::CreateSegCnr(int id, PCornerType cType, float radius, float radiusr, float radiusl, float arc)
	{
		PSeg newSeg = PSeg();

		newSeg.id = id;
		newSeg.type = cType;
		newSeg.radius = radius;
		newSeg.radiusr = radiusr;
		newSeg.radiusl = radiusl;
		newSeg.arc = arc;

		return newSeg;
	}

	PSeg PSegFactory::CreateRandomSeg(int id)
	{
		PSegType type = RandomSegType();

		if (type == PSegType::Straight)
			return CreateSegStr(id, SEG_LENGTH); // Generate new segment
		else
		{
			float radius = RandBetween(ranges.Radius().Min(), ranges.Radius().Max());
			float arc = RandBetween(ranges.Arc().Min(), ranges.Arc().Max());
			PCornerType cType;

			if (previousCornerType == CTLeft)
				cType = CTRight;
			else
				cType = CTLeft;

			return CreateSegCnr(id, cType, radius, 0.f, 0.f, arc);
			previousCornerType = cType;
		}
	}

	void PSegFactory::SetChances(float corner, float straight)
	{
		if (corner + straight == 100)
		{
			cornerChance = corner;
			straightChance = straight;
		}
		else
		{
			taOut("PSegFactory: Error! Corner and straight segment generation chances must equal 100!");
			cornerChance = straightChance = 50.f;
		}
	}

	float PSegFactory::RandBetween(float min, float max)
	{
		if (min < max)
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
		else
			taOut("PSegFactory: Min cannot be less thatn Max!\n");
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