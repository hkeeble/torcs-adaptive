/*
	File: pCarData.cpp
	Author: Henri Keeble
	Desc: Defines a class to be used to monitor a car's data for use in managing a procedurally generated track.
*/
#include "pCarData.h"

namespace procedural
{
	pCarData::pCarData()
	{
		car = nullptr;
		curSeg = nullptr;
		prevSeg = nullptr;
		curTrackPos = tTrkLocPos();
		prevTrackPos = tTrkLocPos();
		dirOfTravel = DirectionOfTravel::FORWARD;
	}

	pCarData::pCarData(tCarElt* car)
	{
		this->car = car;
		curTrackPos = car->pub.trkPos;
		prevTrackPos = curTrackPos;
		dirOfTravel = DirectionOfTravel::FORWARD;
		curSeg = curTrackPos.seg;
		prevSeg = curSeg;
	}

	pCarData::~pCarData()
	{

	}

	void pCarData::Update()
	{
		curTrackPos = car->pub.trkPos;
		curSeg = curTrackPos.seg;

		// If car is closer to the start of the segment than it was previously, or has moved back seg, it is travelling backwards.
		if (curTrackPos.toStart < prevTrackPos.toStart || curSeg->id < prevSeg->id)
			dirOfTravel = DirectionOfTravel::BACKWARD;
		else
			dirOfTravel = DirectionOfTravel::FORWARD;

		// Save currents to previous
		prevSeg = curSeg;
		prevTrackPos = curTrackPos;
	}

	tTrkLocPos pCarData::LocalPosition()
	{
		return curTrackPos;
	}

	DirectionOfTravel pCarData::DirOfTravel()
	{
		return dirOfTravel;
	}

	tTrackSeg* pCarData::CurrentSeg()
	{
		return curSeg;
	}
}