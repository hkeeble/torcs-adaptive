/*
	File: pSeg.cpp
	Author: Henri Keeble
	Desc: Defines a class used to represent a procedural segment, and factory functions to ease instantiation of different types of segment.
*/
#include "pSeg.h"

namespace procedural
{
	pSeg::pSeg()
	{
		id = type = 0;
		length = radius = radiusr = radiusl = arc = 0.f;

		raceInfo = 0x00000000; // Normal Segment
		type2 = 1; // Main Track Segment
		style = 0; // Plan (TR_PLAN)
	}

	pSeg::~pSeg()
	{

	}

	pSegFactory* pSegFactory::instance = NULL;

	pSegFactory::pSegFactory()
	{

	}

	pSegFactory::~pSegFactory()
	{
		if(instance)
			delete instance;
	}

	pSegFactory* pSegFactory::GetInstance()
	{
		if(instance == NULL)
			instance = new pSegFactory();
		return instance;
	}

	pSeg pSegFactory::CreateSegStr(int id, float length)
	{
		pSeg newSeg = pSeg(); // Default Constructor takes care of shared default values

		newSeg.type = 3; // TR_STR #define
		newSeg.id = id;
		newSeg.length = length;

		return newSeg;
	}

	pSeg pSegFactory::CreateSegCnr(int id, pCornerType cType, float radius, float radiusr, float radiusl, float arc)
	{
		pSeg newSeg = pSeg();

		newSeg.id = id;
		newSeg.type = cType;
		newSeg.radius = radius;
		newSeg.radiusr = radiusr;
		newSeg.radiusl = radiusl;
		newSeg.arc = arc;

		return newSeg;
	}


}