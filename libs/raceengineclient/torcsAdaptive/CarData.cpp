/*
	File: CarData.cpp
	Author: Henri Keeble
	Desc: Defines a class to be used to monitor a car's data for use in managing a procedurally generated track.
*/
#include "CarData.h"

CarData::CarData()
{
	car = nullptr;
	segment.previous = nullptr;
	segment.current = nullptr;
	position.current = tTrkLocPos();
	position.previous = tTrkLocPos();
	speed.current = 0.f;
	speed.previous = 0.f;
	dirOfTravel = DirectionOfTravel::FORWARD;
}

CarData::CarData(tCarElt* car)
{
	this->car = car;
	position.current = car->pub.trkPos;
	position.previous = position.current;
	dirOfTravel = DirectionOfTravel::FORWARD;
	segment.current = position.current.seg;
	segment.previous = segment.current;
}

CarData::~CarData()
{

}

void CarData::Update()
{
	position.current = car->pub.trkPos;
	segment.current = position.current.seg;

	// If car is closer to the start of the segment than it was previously, or has moved back seg, it is travelling backwards.
	if (position.current.toStart < position.previous.toStart || segment.current->id < segment.previous->id)
		dirOfTravel = DirectionOfTravel::BACKWARD;
	else
		dirOfTravel = DirectionOfTravel::FORWARD;

	// Save currents to previous
	segment.previous = segment.current;
	position.previous = position.current;
}

tTrkLocPos CarData::LocalPosition()
{
	return position.current;
}

DirectionOfTravel CarData::DirOfTravel()
{
	return dirOfTravel;
}

tTrackSeg* CarData::CurrentSeg()
{
	return segment.current;
}

tCarElt* CarData::GetCar()
{
	return car;
}