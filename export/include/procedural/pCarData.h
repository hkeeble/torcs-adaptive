/*
	File: PCarData.h
	Author: Henri Keeble
	Desc: Declares a class to be used to monitor a car's data for use in managing a procedurally generated track.
*/

#ifndef _P_CAR_DATA_
#define _P_CAR_DATA_

#include "track.h"
#include "car.h"

namespace procedural
{
	/* Used to represent the player's direction of travel */
	enum class DirectionOfTravel
	{
		FORWARD,
		BACKWARD
	};

	/* Represents the car data for use in procedural generation */
	class PCarData
	{
	private:
		tCarElt* car;
		tTrkLocPos curTrackPos;
		tTrkLocPos prevTrackPos;
		DirectionOfTravel dirOfTravel;
		tTrackSeg* curSeg;
		tTrackSeg* prevSeg;
	public:
		PCarData();
		PCarData(tCarElt* car);
		~PCarData();

		/* Update the data stored by using the tCarElt structure */
		void Update();

		/* Retrieve car's current local segment position */
		tTrkLocPos LocalPosition();

		/* Retrieve car's current direction of travel */
		DirectionOfTravel DirOfTravel();

		/* Retrieve the car's current segment data */
		tTrackSeg* CurrentSeg();
	};
}

#endif _P_CAR_DATA_