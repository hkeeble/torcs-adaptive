/*
	File: seg.cpp
	Desc: definitions file for torcsAdaptive segment data
	Author: Henri Keeble
	Created: 06/11/2013
	Last Edit: 06/11/2013
*/
#include "seg.h"

namespace torcsAdaptive
{
	// ----- taSeg Definitions -----
	taSeg::taSeg(int raceInfo, int type, int id, int type2, int style,
		tdble length, tdble radius, tdble radiusr, tdble radiusl, tdble arc)
	{
		this->raceInfo = raceInfo;
		this->type = type;
		this->id = id;
		this->type2 = type2;
		this->style = style;

		// Data 
		this->length = length;

		// For Corners
		this->radius = radius;
		this->radiusr = radiusr;
		this->radiusl = radiusl;
		this->arc = arc;	
	}

	taSeg::~taSeg()
	{

	}
}