/*
	File: CarData.cpp
	Author: Henri Keeble
	Desc: Defines a class to be used to monitor a car's data for use in managing a procedurally generated track.
*/
#include "CarData.h"

CarData::CarData()
{
	car = tCarElt();
}

CarData::CarData(tCarElt* car)
{
	this->car = *car;

	globalPos - Vector2D();
	RtTrackLocal2Global(&this->car.pub.trkPos, &globalPos.x, &globalPos.y, TR_TORIGHT);
	speed = sqrt(car->_speed_x*car->_speed_x + car->_speed_y*car->_speed_y + car->_speed_z*car->_speed_z);
}

CarData::~CarData()
{

}

tTrkLocPos CarData::LocalPosition()
{
	return car.pub.trkPos;
}

Vector2D CarData::GlobalPosition()
{
	return globalPos;
}

tTrackSeg* CarData::CurrentSeg()
{
	return car.pub.trkPos.seg;
}

tCarElt CarData::GetCar()
{
	return car;
}

tdble CarData::Speed() const
{
	return speed;
}