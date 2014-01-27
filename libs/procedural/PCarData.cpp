/*
	File: PCarData.cpp
	Author: Henri Keeble
	Desc: Defines a class to be used to monitor a car's data for use in managing a procedurally generated track.
*/
#include "PCarData.h"

namespace procedural
{
	PCarData::PCarData()
	{
		car = nullptr;
		curSeg = nullptr;
		prevSeg = nullptr;
		curTrackPos = tTrkLocPos();
		prevTrackPos = tTrkLocPos();
		dirOfTravel = DirectionOfTravel::FORWARD;
	}

	PCarData::PCarData(tCarElt* car)
	{
		this->car = car;
		curTrackPos = car->pub.trkPos;
		prevTrackPos = curTrackPos;
		dirOfTravel = DirectionOfTravel::FORWARD;
		curSeg = curTrackPos.seg;
		prevSeg = curSeg;
	}

	PCarData::~PCarData()
	{

	}

	void PCarData::Update()
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

	tTrkLocPos PCarData::LocalPosition()
	{
		return curTrackPos;
	}

	DirectionOfTravel PCarData::DirOfTravel()
	{
		return dirOfTravel;
	}

	tTrackSeg* PCarData::CurrentSeg()
	{
		return curSeg;
	}
}