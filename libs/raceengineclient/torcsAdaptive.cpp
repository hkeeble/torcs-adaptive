/*
	File: torcsAdaptive.cpp
	Desc: definitions file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 05/11/2013
*/

#include "torcsAdaptive.h"

namespace torcsAdaptive
{
	int TaInitTrack(tRmInfo* ReInfo)
	{
		ReInfo->track = new tTrack();

		ReInfo->track->author = "Torcs-Adaptive";
		ReInfo->track->category = "Adaptive Track";
		ReInfo->track->filename = " ";
		ReInfo->track->internalname = "AdaptiveTrack";
		ReInfo->track->name = "Adaptive Track";

		return 0;
	}
}