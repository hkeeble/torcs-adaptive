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

/* Used to represent the player's direction of travel */
enum class DirectionOfTravel
{
	FORWARD,
	BACKWARD
};

/* Represents the car data for use in procedural generation */
class CarData
{
private:
	tCarElt* car;
	tTrkLocPos curTrackPos;
	tTrkLocPos prevTrackPos;
	DirectionOfTravel dirOfTravel;
	tTrackSeg* curSeg;
	tTrackSeg* prevSeg;
public:
	CarData();
	CarData(tCarElt* car);
	~CarData();

	/* Update the data stored by using the tCarElt structure */
	void Update();

	/* Retrieve car's current local segment position */
	tTrkLocPos LocalPosition();

	/* Retrieve car's current direction of travel */
	DirectionOfTravel DirOfTravel();

	/* Retrieve the car's current segment data */
	tTrackSeg* CurrentSeg();
};

#endif // _CAR_DATA_