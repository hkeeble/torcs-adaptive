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
		style = 0; // Plan (TR_PLAN)
	}

	PSeg::~PSeg()
	{

	}

	PSegFactory* PSegFactory::instance = NULL;

	PSegFactory::PSegFactory()
	{

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
}