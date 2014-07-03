/*
	File: CarData.h
	Author: Henri Keeble
	Desc: Declares a class to be used to monitor a car's data. Used in both the procedural and performance measurement
		  libraries, but exported here as it may be useful elsewhere in future.
*/

#ifndef _CAR_DATA_
#define _CAR_DATA_

#include "track.h"
#include "car.h"
#include "robottools.h"

// Used to store 2D vectors for global position
typedef v2t<tdble> Vector2D;

/* Used to represent the player's direction of travel */
enum class DirectionOfTravel
{
	FORWARD,
	BACKWARD
};

/* Represents the car data for use in procedural generation and performance measurement */
class CarData
{
private:
	tCarElt car;
	tdble speed;
	Vector2D globalPos;
public:
	CarData();
	CarData(tCarElt* car);
	~CarData();

	/* Retrieve car's current local segment position */
	tTrkLocPos LocalPosition();

	/* Retrieve car's current global position */
	Vector2D GlobalPosition();

	/* Retrieve car's current direction of travel */
	DirectionOfTravel DirOfTravel();

	/* Retrieve the car's current segment data */
	tTrackSeg* CurrentSeg();

	/* Retrieve pointer the car's struct */
	tCarElt GetCar();

	/* Get speed currently recorded */
	tdble Speed() const;
};

#endif // _CAR_DATA_