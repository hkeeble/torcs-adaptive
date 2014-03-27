/***************************************************************************

    file                 : berniw.cpp
    created              : Mon Apr 17 13:51:00 CET 2000
    copyright            : (C) 2000-2006 by Bernhard Wymann
    email                : berniw@bluewin.ch
    version              : $Id: lliawProc.cpp,v 1.9.2.1 2008/05/31 09:25:28 berniw Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "lliawProc.h"

using namespace procBot;

const double lliawCar::STEER_D_CONTROLLER_GAIN = 0.1;
const float lliawCar::FUEL_SAFETY_MARGIN = 0.15f;
const float lliawCar::CLUTCH_SPEED = 5.0f;				// [m/s]
const float lliawCar::CLUTCH_FULL_MAX_TIME = 2.0f;

// Function prototypes.
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation * situation);
static void drive(int index, tCarElt* car, tSituation *situation);
static void newRace(int index, tCarElt* car, tSituation *situation);
static int  InitFuncPt(int index, void *pt);
static int  pitcmd(int index, tCarElt* car, tSituation *s);
static void update(int index, tSituation *situation);
static void shutdown(int index);
float getClutch(lliawCar* myc, tCarElt* car);

static const char* botname[BOTS] = {
	"lliawProc 1", "lliawProc 2", "lliawProc 3", "lliawProc 4", "lliawProc 5",
	"lliawProc 6", "lliawProc 7", "lliawProc 8", "lliawProc 9", "lliawProc 10"
};

static const char* botdesc[BOTS] = {
	"lliawProc 1", "lliawProc 2", "lliawProc 3", "lliawProc 4", "lliawProc 5",
	"lliawProc 6", "lliawProc 7", "lliawProc 8", "lliawProc 9", "lliawProc 10"
};

// Module entry point.
extern "C" int lliawProc(tModInfo *modInfo)
{
	for (int i = 0; i < BOTS; i++) {
		modInfo[i].name = _strdup(botname[i]);	// Name of the module (short).
		modInfo[i].desc = _strdup(botdesc[i]);	// Description of the module (can be long).
		modInfo[i].fctInit = InitFuncPt;		// Init function.
		modInfo[i].gfId    = ROB_IDENT;			// Supported framework version.
		modInfo[i].index   = i+1;
	}
	return 0;
}


// Initialize function (callback) pointers for torcs.
static int InitFuncPt(int index, void *pt)
{
	tRobotItf *itf = (tRobotItf *)pt;

	itf->rbNewTrack = initTrack;	// Init new track.
	itf->rbNewRace  = newRace;		// Init new race.
	itf->rbDrive    = drive;		// Drive during race.
	itf->rbShutdown	= shutdown;		// Called for cleanup per driver.
	itf->rbUpdate	= update;
	itf->rbPitCmd   = pitcmd;		// Pit command.
	itf->index      = index;
	return 0;
}


static lliawCar* mycar[BOTS] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static POtherCarDesc* ocar = NULL;
static PTrackDesc* myTrackDesc = NULL;
static double currenttime;
static const tdble waitToTurn = 1.0; // How long should i wait till i try to turn backwards.


// Release resources when the module gets unloaded.
static void shutdown(int index) {
	int i = index - 1;
	if (mycar[i] != NULL) {
		delete mycar[i];
		mycar[i] = NULL;
	}
	if (myTrackDesc != NULL) {
		delete myTrackDesc;
		myTrackDesc = NULL;
	}
	if (ocar != NULL) {
		delete [] ocar;
		ocar = NULL;
	}
}


// Initialize track data, called for every selected driver.
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation * situation)
{
	if ((myTrackDesc != NULL) && (myTrackDesc->GetTorcsTrack() != track)) {
		delete myTrackDesc;
		myTrackDesc = NULL;
	}
	if (myTrackDesc == NULL) {
		myTrackDesc = new PTrackDesc(track);
	}

	char buffer[BUFSIZE];
	char* trackname = strrchr(track->filename, '/') + 1;

	switch (situation->_raceType) {
		case RM_TYPE_PRACTICE:
			snprintf(buffer, BUFSIZE, "drivers/lliawProc/%d/practice/%s", index, trackname);
			break;
		case RM_TYPE_QUALIF:
			snprintf(buffer, BUFSIZE, "drivers/lliawProc/%d/qualifying/%s", index, trackname);
			break;
		case RM_TYPE_RACE:
			snprintf(buffer, BUFSIZE, "drivers/lliawProc/%d/race/%s", index, trackname);
			break;
		default:
			break;
	}

	*carParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
	if (*carParmHandle == NULL) {
		snprintf(buffer, BUFSIZE, "drivers/lliawProc/%d/default.xml", index);
		*carParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
    }

	// Load and set parameters.
	float fuel = 10000000000;
}


/* Used to update the car's understanding of the track and pathfinder before the call to the drive function, in a procedural track */
static void update(int index, tSituation *situation)
{
	myTrackDesc->Update(); // Update the track description
	mycar[index - 1]->getPathfinderPtr()->Update(situation); // Update the pathfinder based upon the new track
}

// Initialize driver for the race, called for every selected driver.
static void newRace(int index, tCarElt* car, tSituation *situation)
{
	if (ocar != NULL) {
		delete [] ocar;
	}
	ocar = new POtherCarDesc[situation->_ncars];
	for (int i = 0; i < situation->_ncars; i++) {
		ocar[i].init(myTrackDesc, situation->cars[i], situation);
	}

	if (mycar[index-1] != NULL) {
		delete mycar[index-1];
	}
	mycar[index-1] = new lliawCar(myTrackDesc, car, situation);

	currenttime = situation->currentTime;
}


// Controls the car.
static void drive(int index, tCarElt* car, tSituation *situation)
{
	tdble angle;
	tdble brake;
	tdble b1;							// Brake value in case we are to fast HERE and NOW.
	tdble b2;							// Brake value for some brake point in front of us.
	tdble b3;							// Brake value for control (avoid loosing control).
	tdble b4;							// Brake value for avoiding high angle of attack.
	tdble steer, targetAngle, shiftaccel;

	lliawCar* myc = mycar[index - 1];
	PPathfinder* mpf = myc->getPathfinderPtr();

	b1 = b2 = b3 = b4 = 0.0;
	shiftaccel = 0.0;

	// Compute path according to the situation.
	mpf->dynamicPlan(myc->getCurrentSegId(), car, situation, myc, ocar);

	// Update some values needed.
	myc->update(myTrackDesc, car, situation);

	// Decide how we want to drive, choose a behaviour.
	if ( car->_dammage < myc->undamaged/3 && myc->bmode != myc->NORMAL) {
		myc->loadBehaviour(myc->NORMAL);
	} else if (car->_dammage > myc->undamaged/3 && car->_dammage < (myc->undamaged*2)/3 && myc->bmode != myc->CAREFUL) {
		myc->loadBehaviour(myc->CAREFUL);
	} else if (car->_dammage > (myc->undamaged*2)/3 && myc->bmode != myc->SLOW) {
		myc->loadBehaviour(myc->SLOW);
	}

	// Update the other cars just once.
	if (currenttime != situation->currentTime) {
		currenttime = situation->currentTime;
		for (int i = 0; i < situation->_ncars; i++) ocar[i].update();
	}

	// Startmode.
	if (myc->trtime < 5.0 && myc->bmode != myc->START) {
		myc->loadBehaviour(myc->START);
		myc->startmode = true;
	}
	if (myc->startmode && myc->trtime > 5.0) {
		myc->startmode = false;
		myc->loadBehaviour(myc->NORMAL);
	}

	// Clear ctrl structure with zeros and set the current gear.
	memset(&car->ctrl, 0, sizeof(tCarCtrl));
	car->_gearCmd = car->_gear;

	// Uncommenting the following line causes pitstop on every lap.
	//if (!mpf->getPitStop()) mpf->setPitStop(true, myc->getCurrentSegId());

	// Steer toward the next target point.
	targetAngle = atan2(mpf->getPathSeg(myc->destsegid)->getLoc()->y - car->_pos_Y, mpf->getPathSeg(myc->destsegid)->getLoc()->x - car->_pos_X);
	targetAngle -= car->_yaw;
	NORM_PI_PI(targetAngle);
    steer = targetAngle / car->_steerLock;

	// Try to control angular velocity with a D (differential) controller.
	double omega = myc->getSpeed()/mpf->getPathSeg(myc->getCurrentSegId())->getRadius();
	steer += myc->STEER_D_CONTROLLER_GAIN*(omega - myc->getCarPtr()->_yaw_rate);


	// Brakes.
    tdble brakecoeff = 1.0/(2.0*g*myc->currentseg->getKfriction()*myc->CFRICTION);
    tdble brakespeed, brakedist;
	tdble lookahead = 0.0;
	int i = myc->getCurrentSegId();
	brake = 0.0;

	while (lookahead < brakecoeff * myc->getSpeedSqr()) {
		lookahead += mpf->getPathSeg(i)->getLength();
		brakespeed = myc->getSpeedSqr() - mpf->getPathSeg(i)->getSpeedsqr();
		if (brakespeed > 0.0) {
			tdble gm, qb, qs;
			gm = myTrackDesc->getSegmentPtr(myc->getCurrentSegId())->getKfriction()*myc->CFRICTION*myTrackDesc->getSegmentPtr(myc->getCurrentSegId())->getKalpha();
			qs = mpf->getPathSeg(i)->getSpeedsqr();

			brakedist = brakespeed*(myc->mass/(2.0*gm*g*myc->mass + qs*(gm*myc->ca + myc->cw)));

			if (brakedist > lookahead - myc->getWheelTrack()) {
				qb = brakespeed*brakecoeff/brakedist;
				if (qb > b2) {
					b2 = qb;
				}
			}
		}
		i = (i + 1 + mpf->getnPathSeg()) % mpf->getnPathSeg();
	}

	if (myc->getSpeedSqr() > mpf->getPathSeg(myc->getCurrentSegId())->getSpeedsqr()) {
		b1 = (myc->getSpeedSqr() - mpf->getPathSeg(myc->getCurrentSegId())->getSpeedsqr()) / (myc->getSpeedSqr());
	}

	// Try to avoid flying.
	if (myc->getDeltaPitch() > myc->MAXALLOWEDPITCH && myc->getSpeed() > myc->FLYSPEED) {
		b4 = 1.0;
	}

	// Check if we are on the way.
	if (myc->getSpeed() > myc->TURNSPEED && myc->tr_mode == 0) {
		if (myc->derror > myc->PATHERR) {
			v3d *cd = myc->getDir();
			v3d *pd = mpf->getPathSeg(myc->getCurrentSegId())->getDir();
			float z = cd->x*pd->y - cd->y*pd->x;
			// If the car points away from the path brake.
			if (z*myc->getErrorSgn() >= 0.0) {
				targetAngle = atan2(mpf->getPathSeg(myc->getCurrentSegId())->getDir()->y, mpf->getPathSeg(myc->getCurrentSegId())->getDir()->x);
				targetAngle -= car->_yaw;
				NORM_PI_PI(targetAngle);
				double toborder = MAX(1.0, myc->currentseg->getWidth()/2.0 - fabs(myTrackDesc->distToMiddle(myc->getCurrentSegId(), myc->getCurrentPos())));
				b3 = (myc->getSpeed()/myc->STABLESPEED)*(myc->derror-myc->PATHERR)/toborder;
			}
		}
	}

	// Anti wheel locking and brake code.
	if (b1 > b2) brake = b1; else brake = b2;
	if (brake < b3) brake = b3;
	if (brake < b4) {
		brake = MIN(1.0, b4);
		tdble abs_mean;
		abs_mean = (car->_wheelSpinVel(REAR_LFT) + car->_wheelSpinVel(REAR_RGT))*car->_wheelRadius(REAR_LFT)/myc->getSpeed();
		abs_mean /= 2.0;
    	brake = brake * abs_mean;
	} else {
		brake = MIN(1.0, brake);
		tdble abs_min = 1.0;
		for (int i = 0; i < 4; i++) {
			tdble slip = car->_wheelSpinVel(i) * car->_wheelRadius(i) / myc->getSpeed();
			if (slip < abs_min) abs_min = slip;
		}
    	brake = brake * abs_min;
	}

	// Reduce brake value to the approximate normal force available on the wheels.
	float weight = myc->mass*G;
	float maxForce = weight + myc->ca*myc->MAX_SPEED*myc->MAX_SPEED;
	float force = weight + myc->ca*myc->getSpeedSqr();
	brake = brake*MIN(1.0, force/maxForce);


	// Gear changing.
	if (myc->tr_mode == 0) {
		if (car->_gear <= 0) {
			car->_gearCmd =  1;
		} else {
			float gr_up = car->_gearRatio[car->_gear + car->_gearOffset];
			float omega = car->_enginerpmRedLine/gr_up;
			float wr = car->_wheelRadius(2);

			if (omega*wr*myc->SHIFT < car->_speed_x) {
				car->_gearCmd++;
			} else {
				float gr_down = car->_gearRatio[car->_gear + car->_gearOffset - 1];
				omega = car->_enginerpmRedLine/gr_down;
				if (car->_gear > 1 && omega*wr*myc->SHIFT > car->_speed_x + myc->SHIFT_MARGIN) {
					car->_gearCmd--;
				}
			}
		}
	}


	// Acceleration / brake execution.
	tdble cerror, cerrorh;
	cerrorh = sqrt(car->_speed_x*car->_speed_x + car->_speed_y*car->_speed_y);
	if (cerrorh > myc->TURNSPEED) {
		cerror = fabs(car->_speed_x)/cerrorh;
	} else {
		cerror = 1.0;
	}

	if (myc->tr_mode == 0) {
		if (brake > 0.0) {
			myc->accel = 0.0;
			car->_accelCmd = myc->accel;
			car->_brakeCmd = brake*cerror;
		} else {
			if (myc->getSpeedSqr() < mpf->getPathSeg(myc->getCurrentSegId())->getSpeedsqr()) {
				if (myc->accel < myc->ACCELLIMIT) {
					myc->accel += myc->ACCELINC;
				}
				car->_accelCmd = myc->accel/cerror;
			} else {
				if (myc->accel > 0.0) {
					myc->accel -= myc->ACCELINC;
				}
				// TODO: shiftaccel always 0 at the moment...
				car->_accelCmd = myc->accel = MIN(myc->accel/cerror, shiftaccel/cerror);
			}
			tdble slipspeed = myc->querySlipSpeed(car);
			if (slipspeed > myc->TCL_SLIP) {
				car->_accelCmd = car->_accelCmd - MIN(car->_accelCmd, (slipspeed - myc->TCL_SLIP)/myc->TCL_RANGE);
			}
		}
	}


	// Check if we are stuck, try to get unstuck.
	tdble bx = myc->getDir()->x, by = myc->getDir()->y;
	tdble cx = myc->currentseg->getMiddle()->x - car->_pos_X, cy = myc->currentseg->getMiddle()->y - car->_pos_Y;
	tdble parallel = (cx*bx + cy*by) / (sqrt(cx*cx + cy*cy)*sqrt(bx*bx + by*by));

	if ((myc->getSpeed() < myc->TURNSPEED) && (parallel < cos(90.0*PI / 180.0)) && (mpf->dist2D(myc->getCurrentPos(), mpf->getPathSeg(myc->getCurrentSegId())->getLoc()) > myc->TURNTOL)) {
		myc->turnaround += situation->deltaTime;
	} else myc->turnaround = 0.0;
	if ((myc->turnaround >= waitToTurn) || (myc->tr_mode >= 1)) {
		if (myc->tr_mode == 0) {
			myc->tr_mode = 1;
		}
        if ((car->_gearCmd > -1) && (myc->tr_mode < 2)) {
			car->_accelCmd = 0.0;
			if (myc->tr_mode == 1) {
				car->_gearCmd--;
			}
			car->_brakeCmd = 1.0;
		} else {
			myc->tr_mode = 2;
			if (parallel < cos(90.0*PI / 180.0) && (mpf->dist2D(myc->getCurrentPos(), mpf->getPathSeg(myc->getCurrentSegId())->getLoc()) > myc->TURNTOL)) {
				angle = queryAngleToTrack(car);
				car->_steerCmd = ( -angle > 0.0) ? 1.0 : -1.0;
				car->_brakeCmd = 0.0;

				if (myc->accel < 1.0) {
					myc->accel += myc->ACCELINC;
				}
				car->_accelCmd = myc->accel;
				tdble slipspeed = myc->querySlipSpeed(car);
				if (slipspeed < -myc->TCL_SLIP) {
					car->_accelCmd = car->_accelCmd - MIN(car->_accelCmd, (myc->TCL_SLIP - slipspeed)/myc->TCL_RANGE);
				}
			} else {
				if (myc->getSpeed() < 1.0) {
					myc->turnaround = 0;
					myc->tr_mode = 0;
					myc->loadBehaviour(myc->START);
					myc->startmode = true;
					myc->trtime = 0.0;
				}
				car->_brakeCmd = 1.0;
				car->_steerCmd = 0.0;
				car->_accelCmd = 0.0;
			}
		}
	}

	if (myc->tr_mode == 0) car->_steerCmd = steer;
	car->_clutchCmd = getClutch(myc, car);
}


// Pitstop callback.
static int pitcmd(int index, tCarElt* car, tSituation *s)
{
	lliawCar* myc = mycar[index-1];
	PPathfinder* mpf = myc->getPathfinderPtr();

	float fullracedist = (myTrackDesc->GetTorcsTrack()->length*s->_totLaps);
	float remaininglaps = (fullracedist - car->_distRaced)/myTrackDesc->GetTorcsTrack()->length;

	car->_pitFuel = MAX(MIN(myc->fuelperlap*(remaininglaps+myc->FUEL_SAFETY_MARGIN) - car->_fuel, car->_tank - car->_fuel), 0.0);
	myc->lastpitfuel = MAX(car->_pitFuel, 0.0);
	car->_pitRepair = car->_dammage;
	mpf->setPitStop(false, myc->getCurrentSegId());
	myc->loadBehaviour(myc->START);
	myc->startmode = true;
	myc->trtime = 0.0;

	return ROB_PIT_IM; // Return immediately.
}


// Compute the clutch value.
// Does not work great when braking to 0 and accelerating again...
// TODO: Improve.
float getClutch(lliawCar* myc, tCarElt* car)
{
	if (car->_gear > 1) {
		myc->clutchtime = 0.0;
		return 0.0;
	} else {
		float drpm = car->_enginerpm - car->_enginerpmRedLine/2.0;
		myc->clutchtime = MIN(myc->CLUTCH_FULL_MAX_TIME, myc->clutchtime);
		float clutcht = (myc->CLUTCH_FULL_MAX_TIME - myc->clutchtime)/myc->CLUTCH_FULL_MAX_TIME;
		if (car->_gear == 1 && car->_accelCmd > 0.0) {
			myc->clutchtime += (float) RCM_MAX_DT_ROBOTS;
		}

		//printf("ct: %f, car->_gear: %d, car->_gearCmd: %d, drpm: %f\n", myc->clutchtime, car->_gear, car->_gearCmd, drpm);
		if (drpm > 0) {
			float speedr;
			if (car->_gearCmd == 1) {
				// Compute corresponding speed to engine rpm.
				float omega = car->_enginerpmRedLine/car->_gearRatio[car->_gear + car->_gearOffset];
				float wr = car->_wheelRadius(2);
				speedr = (myc->CLUTCH_SPEED + MAX(0.0, car->_speed_x))/fabs(wr*omega);
				float clutchr = MAX(0.0, (1.0 - speedr*2.0*drpm/car->_enginerpmRedLine));
				return MIN(clutcht, clutchr);
			} else {
				// For the reverse gear.
				myc->clutchtime = 0.0;
				return 0.0;
			}
		} else {
			return clutcht;
		}
	}
}