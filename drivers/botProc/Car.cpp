/*
	File: Car.cpp
	Author: Henri Keeble
	Desc: A class to represent the car in the botProc AI driver.
*/

#include "Car.h"

const tdble Car::MAX_SPEED = 84.0;

void Car::Update()
{
	memset(&car->ctrl, 0, sizeof(tCarCtrl));

	currentSegment = car->pub.trkPos.seg;

	// Calculate values
	CalcSteerAngle();
	CalcSpeed();

	// Update controls
	SetAccel();
	SetBrake();
	SetGear();
}

void Car::CalcSteerAngle()
{
	const float SC = 1.0;

	steerAngle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
	NORM_PI_PI(steerAngle); // put the angle back in the range from -PI to PI
	steerAngle -= SC*car->_trkPos.toMiddle / car->_trkPos.seg->width;

	// set up the values to return
	car->ctrl.steer = steerAngle / car->_steerLock;
}

void Car::CalcSpeed()
{
	if (currentSegment->next->type != TR_RGT && currentSegment->next->type != TR_LFT)
		optSpeed = MAX_SPEED;
	else
		optSpeed = 20.f;
}

void Car::SetGear()
{
	car->ctrl.gear = 1;
}

void Car::SetAccel()
{
	if (GetSpeed()*0.44704 < MAX_SPEED)
		car->ctrl.accelCmd = 1.0;
}

void Car::SetBrake()
{
	if (GetSpeed()*0.44704 > MAX_SPEED)
		car->ctrl.brakeCmd = 1.0;
}