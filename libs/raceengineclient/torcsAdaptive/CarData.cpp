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
	localPos.current = tTrkLocPos();
	localPos.previous = tTrkLocPos();
	globalPos.current = Vector2D();
	globalPos.previous = Vector2D();
	speed.current = 0.f;
	speed.previous = 0.f;
	dirOfTravel = DirectionOfTravel::FORWARD;
}

CarData::CarData(tCarElt* car)
{
	this->car = car;
	localPos.current = car->pub.trkPos;
	localPos.previous = localPos.current;
	dirOfTravel = DirectionOfTravel::FORWARD;
	segment.current = localPos.current.seg;
	segment.previous = segment.current;
}

CarData::~CarData()
{

}

void CarData::Update()
{
	// Update data
	localPos.current = car->pub.trkPos;
	segment.current = localPos.current.seg;
	speed.current = speed.CalculateSpeed(car->_speed_x, car->_speed_y, car->_speed_z);

	// Record global position, converting local to global through use of robot tools function
	tTrkLocPos* tmp = new tTrkLocPos(localPos.current);
	RtTrackLocal2Global(tmp, &globalPos.current.x, &globalPos.current.y, TR_TORIGHT);
	delete tmp;

	// If car is closer to the start of the segment than it was previously, or has moved back seg, it is travelling backwards.
	if (localPos.current.toStart < localPos.previous.toStart || segment.current->id < segment.previous->id)
		dirOfTravel = DirectionOfTravel::BACKWARD;
	else
		dirOfTravel = DirectionOfTravel::FORWARD;

	// Save currents to previous
	segment.previous = segment.current;
	localPos.previous = localPos.current;
	speed.previous = speed.current;
	globalPos.previous = globalPos.current;
}

tTrkLocPos CarData::LocalPosition()
{
	return localPos.current;
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

tdble CarData::Speed() const
{
	return speed.current;
}