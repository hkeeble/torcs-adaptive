/***************************************************************************

    file                 : berniw.h
    created              : Mon Mai 15 16:52:00 CET 2000
    copyright            : (C) 2000-2006 by Bernhard Wymann
    email                : berniw@bluewin.ch
    version              : $Id: berniw.h,v 1.1.2.2 2008/11/09 17:50:20 berniw Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _BERNIW_H_
#define _BERNIW_H_

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
#include <tmath/linalg_t.h>
#include <portability.h>
#include "PTrackDesc.h"
#include "PCarDesc.h"
#include "PPathfinder.h"

#define TRACKFILENAME		"/home/berni/track.dat"
#define TRACKDESCFILENAME   "/home/berni/track.txt"
#define RMAX				10000.0
#define BOTS 10
#define BUFSIZE 256

#define BERNIW_SECT_PRIV			"berniw private"
#define BERNIW_ATT_PITENTRY			"pitentry"
#define BERNIW_ATT_PITEXIT			"pitexit"
#define BERNIW_ATT_AMAGIC			"caero"
#define BERNIW_ATT_FMAGIC			"cfriction"
#define BERNIW_ATT_FUELPERLAP		"fuelperlap"
#define BERNIW_ATT_STEERPGAIN		"steerpgain"
#define BERNIW_ATT_STEERPGAIN_MAX	"steerpgainmax"
#define BERNIW_ATT_TEAMMATE			"teammate"

using namespace procPathfinder;

/* Extended car desc - has some more static parameters */
class lliawCar : public PCarDesc
{
public:
	lliawCar(tTrack* itrack, tCarElt* icar, tSituation *situation) : PCarDesc(itrack, icar, situation) { };
	virtual ~lliawCar() { };

	/* Additional statics */
	static const float FUEL_SAFETY_MARGIN;
	static const double STEER_D_CONTROLLER_GAIN;
	static const float CLUTCH_FULL_MAX_TIME;
	static const float CLUTCH_SPEED;

	/* Additional values  */
	double STEER_P_CONTROLLER_MAX;					// [-] Maximum output of p-controller.
	double STEER_P_CONTROLLER_GAIN;					// [-] Output gain of p-controller.

	float clutchtime;		// Clutch timer.
};

/* compute the angle between the track and the car */
inline double queryAngleToTrack(tCarElt * car)
{
	double angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
	NORM_PI_PI(angle);
	return angle;
}

#endif // _BERNIW_H_

