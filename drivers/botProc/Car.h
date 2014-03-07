/*
	File: Car.h
	Author: Henri Keeble
	Desc: A class to represent the car in the botProc AI driver.
*/

#ifndef _CAR_H_
#define _CAR_H_

#include <track.h>
#include <car.h>
#include <robottools.h>

class Car
{
public:
	Car() : track(nullptr), car(nullptr), currentSegment(nullptr), steerAngle(0.f), optSpeed(0.f){ }
	~Car() { };

	void Update();

	/* Get the current segment. */
	tTrackSeg* GetCurrentSegment() const { return currentSegment; }

	/* Get the currently calculated steer angle. */
	tdble GetSteerAngle() const { return steerAngle; }

	/* Set the current track */
	void SetTrack(tTrack* theTrack) { track = theTrack; }

	/* Set the current car */
	void SetCar(tCarElt* theCar) { car = theCar; }

private:
	tCarElt* car;
	tTrack* track;

	tdble GetSpeed() { return (car->_speed_x)*(car->_speed_x) + (car->_speed_y)*(car->_speed_y) + (car->_speed_z)*(car->_speed_z); }

	void CalcSteerAngle();
	void CalcSpeed();

	void SetGear();
	void SetAccel();
	void SetBrake();

	tTrackSeg* currentSegment;
	tdble steerAngle;
	tdble optSpeed;

	// Static Constants
	static const tdble MAX_SPEED;
};

#endif _CAR_H_