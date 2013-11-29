/*
	File: taSeg.cpp
	Desc: definitions file for torcsAdaptive segments
	Author: Henri Keeble
*/
#include "torcsAdaptive\taSeg.h"

namespace torcsAdaptive
{
	taSeg::taSeg()
	{
		id = type = 0;
		length = radius = radiusr = radiusl = arc = 0.f;

		raceInfo = 0x00000000; // Normal Segment
		type2 = 1; // Main Track Segment
		style = 0; // Plan (TR_PLAN)
	}

	taSeg::~taSeg()
	{

	}

	TaSegFactory* TaSegFactory::instance = NULL;

	TaSegFactory::TaSegFactory()
	{

	}

	TaSegFactory::~TaSegFactory()
	{
		if(instance)
			delete instance;
	}

	TaSegFactory* TaSegFactory::GetInstance()
	{
		if(instance == NULL)
			instance = new TaSegFactory();
		return instance;
	}

	taSeg TaSegFactory::CreateSegStr(int id, float length)
	{
		taSeg newSeg = taSeg(); // Default Constructor takes care of shared default values

		newSeg.type = 3; // TR_STR #define
		newSeg.id = id;
		newSeg.length = length;

		return newSeg;
	}

	taSeg TaSegFactory::CreateSegCnr(int id, TaCornerType cType, float radius, float radiusr, float radiusl, float arc)
	{
		taSeg newSeg = taSeg();

		newSeg.id = id;
		newSeg.type = cType;
		newSeg.radius = radius;
		newSeg.radiusr = radiusr;
		newSeg.radiusl = radiusl;
		newSeg.arc = arc;

		return newSeg;
	}


}