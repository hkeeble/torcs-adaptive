/*
	File: CarData.cpp
	Author: Henri Keeble
	Desc: Defines a class to be used to monitor a car's data for use in managing a procedurally generated track.
*/
#include "CarData.h"

CarData::CarData()
{
	car = nullptr;
	curSeg = nullptr;
	prevSeg = nullptr;
	curTrackPos = tTrkLocPos();
	prevTrackPos = tTrkLocPos();
	dirOfTravel = DirectionOfTravel::FORWARD;
}

CarData::CarData(tCarElt* car)
{
	this->car = car;
	curTrackPos = car->pub.trkPos;
	prevTrackPos = curTrackPos;
	dirOfTravel = DirectionOfTravel::FORWARD;
	curSeg = curTrackPos.seg;
	prevSeg = curSeg;
}

CarData::~CarData()
{

}

void CarData::Update()
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

tTrkLocPos CarData::LocalPosition()
{
	return curTrackPos;
}

DirectionOfTravel CarData::DirOfTravel()
{
	return dirOfTravel;
}

tTrackSeg* CarData::CurrentSeg()
{
	return curSeg;
}