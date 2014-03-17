/*
	File: PathSeg.cpp
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Modified version of the path segments from a pathfinder object from the TORCS robot berniw for a procedural track.
*/

#include "PathSeg.h"

namespace procBot
{
	PathSeg::PathSeg(v3d location, v3d optimalLocation, tdble speedsqr, tdble weight, tdble radius)
	{
		this->speedsqr = speedsqr;
		this->p = location;
		this->o = optimalLocation;
		this->d = p;
		this->weight = weight;
		this->radius = radius;

		// Normalize direction vectory for dynamic trajectory
		d.normalize();
	}
}