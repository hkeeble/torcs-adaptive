/*
    File: berniProc.xml
    Author: Bernhard Wymann
    Modified by: Henri Keeble
	Desc: Modified version of the robot berniw for TORCS adaptive.
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _BERNI_PROC_H_
#define _BERNI_PROC_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robot.h>
#include <robottools.h>
#include <math.h>
#include "spline.h"
#include "PTrackDesc.h"
#include "PCarDesc.h"
#include "PPathfinder.h"

#define TRACKFILENAME		"/home/berniProc/track.dat"
#define TRACKDESCFILENAME   "/home/berniProc/track.txt"
#define RMAX				10000.0
#define BOTS			    10
#define BUFSIZE				256

#define BERNIW_SECT_PRIV		"berniProc private"
#define BERNIW_ATT_PITENTRY		"pitentry"
#define	BERNIW_ATT_PITEXIT		"pitexit"
#define BERNIW_ATT_AMAGIC		"caero"
#define BERNIW_ATT_FMAGIC		"cfriction"
#define	BERNIW_ATT_FUELPERLAP	"fuelperlap"

/* compute the angle between the track and the car */
inline double queryAngleToTrack(tCarElt * car)
{
	double angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
	NORM_PI_PI(angle);
	return angle;
}

#endif // _BERNI_PROC_H_

