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

/* Contains data about car's speed */
struct SpeedData
{
private:
	friend class CarData;
	SpeedData() { }
	tdble current;
	tdble previous;
	tdble CalculateSpeed(tdble x, tdble y, tdble z) { return sqrt((x*x) + (y*y) + (z*z)); }
};

/* Contains local position data */
struct PositionData
{
private:
	friend class CarData;
	PositionData() { }
	tTrkLocPos current;
	tTrkLocPos previous;
};

/* Contains segment data */
struct SegData
{
private:
	friend class CarData;
	SegData() { }
	tTrackSeg* current;
	tTrackSeg* previous;
};

/* Represents the car data for use in procedural generation */
class CarData
{
private:
	tCarElt* car;
	SpeedData speed;
	PositionData position;
	SegData segment;
	DirectionOfTravel dirOfTravel;
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

	/* Retrieve pointer the car's struct */
	tCarElt* GetCar();

	/* Get speed currently recorded */
	tdble Speed() const;
};

#endif // _CAR_DATA_