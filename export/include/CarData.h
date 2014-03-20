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
struct LocalPositionData
{
private:
	friend class CarData;
	LocalPositionData() { }
	tTrkLocPos current;
	tTrkLocPos previous;
};

struct GlobalPositionData
{
private:
	friend class CarData;
	GlobalPositionData() { }
	Vector2D current;
	Vector2D previous;
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


/* Represents the car data for use in procedural generation and performance measurement */
class CarData
{
private:
	tCarElt* car;
	SpeedData speed;
	GlobalPositionData globalPos;
	LocalPositionData localPos;
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

	/* Retrieve car's current global position */
	Vector2D GlobalPosition();

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