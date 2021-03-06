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


#include "berniProc.h"
#include "taMath\taMath.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

using namespace procPathfinder;

/* function prototypes */
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation * situation);
static void drive(int index, tCarElt* car, tSituation *situation);
static void newRace(int index, tCarElt* car, tSituation *situation);
static int  InitFuncPt(int index, void *pt);
static int  pitcmd(int index, tCarElt* car, tSituation *s);
static void update(int index, tSituation *situation);
static void shutdown(int index);


static const char* botname[BOTS] = {
	"berniProc 1", "berniProc 2", "berniProc 3", "berniProc 4", "berniProc 5",
	"berniProc 6", "berniProc 7", "berniProc 8", "berniProc 9", "berniProc 10"
};

static const char* botdesc[BOTS] = {
	"berniProc 1", "berniProc 2", "berniProc 3", "berniProc 4", "berniProc 5",
	"berniProc 6", "berniProc 7", "berniProc 8", "berniProc 9", "berniProc 10"
};

/* Module entry point */
extern "C" int berniProc(tModInfo *modInfo)
{
	//char	buffer[BUFSIZE];

	for (int i = 0; i < BOTS; i++) {
		modInfo[i].name = strdup(botname[i]);	/* name of the module (short) */
		modInfo[i].desc = strdup(botdesc[i]);	/* description of the module (can be long) */
		modInfo[i].fctInit = InitFuncPt;		/* init function */
		modInfo[i].gfId = ROB_IDENT;			/* supported framework version */
		modInfo[i].index = i + 1;
	}
	return 0;
}


/* initialize function (callback) pointers for torcs */
static int InitFuncPt(int index, void *pt)
{
	tRobotItf *itf = (tRobotItf *)pt;

	itf->rbNewTrack = initTrack;	/* init new track */
	itf->rbNewRace = newRace;		/* init new race */
	itf->rbDrive = drive;		/* drive during race */
	itf->rbShutdown = shutdown;		/* called for cleanup per driver */
	itf->rbPitCmd = pitcmd;		/* pit command */
	itf->rbUpdate = update; /* Used to update bot's understanding of the track */
	itf->index = index;
	return 0;
}


static PCarDesc* mycar[BOTS] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
static POtherCarDesc* ocar = nullptr;

static tTrack* currentTrack = nullptr;
static PTrackDesc* myTrackDesc = nullptr;
static PPathfinder* myPathfinder = nullptr;
static PPathPlanner* myPathPlanner = nullptr;

static int segsInCurrentTrack = 0;
static double currenttime;
static const tdble waitToTurn = 1.0; /* how long should i wait till i try to turn backwards */


/* release resources when the module gets unloaded */
static void shutdown(int index) {
	int i = index - 1;
	if (mycar[i] != nullptr) {
		delete mycar[i];
		mycar[i] = nullptr;
		//free(botdesc[i]);
		//free(botname[i]);
	}
	if (ocar != nullptr) {
		delete[] ocar;
		ocar = nullptr;
	}
	if (myTrackDesc != nullptr) {
		delete myTrackDesc;
		myTrackDesc = nullptr;
	}
	if (myPathfinder != nullptr) {
		delete myPathfinder;
		myPathfinder = nullptr;
	}
	if (myPathPlanner != nullptr) {
		delete myPathPlanner;
		myPathPlanner = nullptr;
	}
}


/* initialize track data, called for every selected driver */
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation * situation)
{
	// Obtain pointer to the current track
	currentTrack = track;

	if (myTrackDesc != nullptr) {
		delete myTrackDesc;
	}
	if (myPathfinder != nullptr) {
		delete myPathfinder;
	}

	myTrackDesc = new PTrackDesc(currentTrack);
	myPathfinder = new K1999(myTrackDesc);

	// Initialize number of segments
	segsInCurrentTrack = currentTrack->nseg;

	char buffer[BUFSIZE];
	char* trackname = track->internalname;

	_snprintf(buffer, BUFSIZE, "drivers/berniProc/%d/%s", index, trackname);
	*carParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);

	if (*carParmHandle == nullptr) {
		_snprintf(buffer, BUFSIZE, "drivers/berniProc/%d/default.xml", index);
		*carParmHandle = GfParmReadFile(buffer, GFPARM_RMODE_STD);
	}

	/* Load and set parameters */
	float fuel = 10000000000;
	GfParmSetNum(*carParmHandle, SECT_CAR, PRM_FUEL, (char*)nullptr, MIN(fuel, 100.0));
}


/* initialize driver for the race, called for every selected driver */
static void newRace(int index, tCarElt* car, tSituation *situation)
{
	if (mycar[index - 1] != nullptr) delete mycar[index - 1];
	mycar[index - 1] = new PCarDesc(myPathfinder, car, situation);

	if (ocar != nullptr) {
		delete[] ocar;
	}
	ocar = new POtherCarDesc[situation->_ncars];
	for (int i = 0; i < situation->_ncars; i++) {
		ocar[i].init(myTrackDesc, situation->cars[i], situation);
	}

	if (myPathPlanner != nullptr) {
		delete myPathPlanner;
	}

	myPathPlanner = new PPathPlanner(myPathfinder->Segs(), myTrackDesc, situation);

	currenttime = situation->currentTime;
}

/* Used to update the car's understanding of the track and pathfinder before the call to the drive function, in a procedural track */
static void update(int index, tSituation *situation)
{
	myTrackDesc->Update();
	myPathfinder->Update(mycar[index - 1]);
	myPathPlanner->Update(myPathfinder->Segs(), myPathfinder->LookAhead());
}

/* controls the car */
static void drive(int index, tCarElt* car, tSituation *situation)
{
	tdble angle;
	tdble brake;
	tdble b1;							/* brake value in case we are to fast HERE and NOW */
	tdble b2;							/* brake value for some brake point in front of us */
	tdble b3;							/* brake value for control (avoid loosing control) */
	tdble b4;							/* brake value for avoiding high angle of attack */
	tdble steer, targetAngle, shiftaccel;

	// Obtain pointers to path finder and path planner
	PCarDesc* myc = mycar[index - 1];

	b1 = b2 = b3 = b4 = 0.0;
	shiftaccel = 0.0;

	/* compute dynamic path according to the situation */
	myPathPlanner->Plan(myc->getCurrentSegId(), car, situation, myc, ocar);

	/* update some values needed */
	myc->update(myTrackDesc, car, situation);

	/* decide how we want to drive */
	if (car->_dammage < myc->undamaged / 3 && myc->bmode != myc->NORMAL) {
		myc->loadBehaviour(myc->NORMAL);
	}
	else if (car->_dammage > myc->undamaged / 3 && car->_dammage < (myc->undamaged * 2) / 3 && myc->bmode != myc->CAREFUL) {
		myc->loadBehaviour(myc->CAREFUL);
	}
	else if (car->_dammage >(myc->undamaged * 2) / 3 && myc->bmode != myc->SLOW) {
		myc->loadBehaviour(myc->SLOW);
	}

	/* update the other cars just once */
	if (currenttime != situation->currentTime) {
		currenttime = situation->currentTime;
		for (int i = 0; i < situation->_ncars; i++) ocar[i].update();
	}

	/* startmode */
	if (myc->trtime < 5.0 && myc->bmode != myc->START) {
		myc->loadBehaviour(myc->START);
		myc->startmode = true;
	}
	if (myc->startmode && myc->trtime > 5.0) {
		myc->startmode = false;
		myc->loadBehaviour(myc->NORMAL);
	}

	/* clear ctrl structure with zeros and set the current gear */
	memset(&car->ctrl, 0, sizeof(tCarCtrl));
	car->_gearCmd = car->_gear;

	/* uncommenting the following line causes pitstop on every lap */
	//if (!mpf->getPitStop()) mpf->setPitStop(true, myc->getCurrentSegId());
	/* compute fuel consumption */
	if (myc->getCurrentSegId() >= 0 && myc->getCurrentSegId() < 5 && !myc->fuelchecked) {
		if (car->race.laps > 0) {
			myc->fuelperlap = MAX(myc->fuelperlap, (myc->lastfuel + myc->lastpitfuel - car->priv.fuel));
		}
		myc->lastfuel = car->priv.fuel;
		myc->lastpitfuel = 0.0;
		myc->fuelchecked = true;
	}
	else if (myc->getCurrentSegId() > 5) {
		myc->fuelchecked = false;
	}

	/* steer to next target point */
	targetAngle = atan2(myc->destpathseg->getLoc()->y - car->_pos_Y, myc->destpathseg->getLoc()->x - car->_pos_X);
	targetAngle -= car->_yaw;
	NORM_PI_PI(targetAngle);
	steer = targetAngle / car->_steerLock;

	/* brakes */
	tdble brakecoeff = 1.0 / (2.0*g*myc->currentseg->getKfriction()*myc->CFRICTION);
	tdble brakespeed, brakedist;
	tdble lookahead = 0.0;
	int i = myc->getCurrentSegId();
	brake = 0.0;

	while (lookahead < brakecoeff * myc->getSpeedSqr()) {
		lookahead += myPathfinder->Seg(i)->getLength();
		brakespeed = myc->getSpeedSqr() - myPathfinder->Seg(i)->getSpeedsqr();
		if (brakespeed > 0.0) {
			tdble gm, qb, qs;
			gm = myTrackDesc->getSegmentPtr(myc->getCurrentSegId())->getKfriction()*myc->CFRICTION*myTrackDesc->getSegmentPtr(myc->getCurrentSegId())->getKalpha();
			qs = myPathfinder->Seg(i)->getSpeedsqr();
			brakedist = brakespeed*(myc->mass / (2.0*gm*g*myc->mass + qs*(gm*myc->ca + myc->cw)));

			if (brakedist > lookahead - myc->getWheelTrack()) {
				qb = brakespeed*brakecoeff / brakedist;
				if (qb > b2) {
					b2 = qb;
				}
			}
		}
		i = (i + 1 + myPathfinder->getnPathSeg()) % myPathfinder->getnPathSeg();
	}

	if (myc->getSpeedSqr() > myc->currentpathseg->getSpeedsqr()) {
		b1 = (myc->getSpeedSqr() - myc->currentpathseg->getSpeedsqr()) / (myc->getSpeedSqr());
	}

	/* try to avoid flying */
	if (myc->getDeltaPitch() > myc->MAXALLOWEDPITCH && myc->getSpeed() > myc->FLYSPEED) {
		b4 = 1.0;
	}

	steer = steer + MIN(0.1, myc->derror*0.02)*myc->getErrorSgn();
	if (fabs(steer) > 1.0) steer /= fabs(steer);

	/* check if we are on the way */
	if (myc->getSpeed() > myc->TURNSPEED && myc->tr_mode == 0) {
		if (myc->derror > myc->PATHERR) {
			v3d r;
			myc->getDir()->crossProduct(myc->currentpathseg->getDir(), &r);
			if (r.z*myc->getErrorSgn() >= 0.0) {
				targetAngle = atan2(myc->currentpathseg->getDir()->y, myc->currentpathseg->getDir()->x);
				targetAngle -= car->_yaw;
				NORM_PI_PI(targetAngle);
				double toborder = MAX(1.0, myc->currentseg->getWidth() / 2.0 - fabs(myTrackDesc->distToMiddle(myc->getCurrentSegId(), myc->getCurrentPos())));
				b3 = (myc->getSpeed() / myc->STABLESPEED)*(myc->derror - myc->PATHERR) / toborder;
			}
		}
	}

	/* try to control angular velocity */
	double omega = myc->getSpeed() / myc->currentpathseg->getRadius();
	steer += 0.1*(omega - myc->getCarPtr()->_yaw_rate);

	/* anti blocking and brake code */
	if (b1 > b2) brake = b1; else brake = b2;
	if (brake < b3) brake = b3;
	if (brake < b4) {
		brake = MIN(1.0, b4);
		tdble abs_mean;
		abs_mean = (car->_wheelSpinVel(REAR_LFT) + car->_wheelSpinVel(REAR_RGT))*car->_wheelRadius(REAR_LFT) / myc->getSpeed();
		abs_mean /= 2.0;
		brake = brake * abs_mean;
	}
	else {
		brake = MIN(1.0, brake);
		tdble abs_min = 1.0;
		for (int i = 0; i < 4; i++) {
			tdble slip = car->_wheelSpinVel(i) * car->_wheelRadius(i) / myc->getSpeed();
			if (slip < abs_min) abs_min = slip;
		}
		brake = brake * abs_min;
	}

	float weight = myc->mass*G;
	float maxForce = weight + myc->ca*myc->MAX_SPEED*myc->MAX_SPEED;
	float force = weight + myc->ca*myc->getSpeedSqr();
	brake = brake*MIN(1.0, force / maxForce);

	// Gear changing.
	if (myc->tr_mode == 0) {
		if (car->_gear <= 0) {
			car->_gearCmd = 1;
		}
		else {
			float gr_up = car->_gearRatio[car->_gear + car->_gearOffset];
			float omega = car->_enginerpmRedLine / gr_up;
			float wr = car->_wheelRadius(2);

			if (omega*wr*myc->SHIFT < car->_speed_x) {
				car->_gearCmd++;
			}
			else {
				float gr_down = car->_gearRatio[car->_gear + car->_gearOffset - 1];
				omega = car->_enginerpmRedLine / gr_down;
				if (car->_gear > 1 && omega*wr*myc->SHIFT > car->_speed_x + myc->SHIFT_MARGIN) {
					car->_gearCmd--;
				}
			}
		}
	}

	tdble cerror, cerrorh;
	cerrorh = sqrt(car->_speed_x*car->_speed_x + car->_speed_y*car->_speed_y);
	if (cerrorh > myc->TURNSPEED) cerror = fabs(car->_speed_x) / cerrorh; else cerror = 1.0;

	/* acceleration / brake execution */
	if (myc->tr_mode == 0) {
		if (brake > 0.0) {
			myc->accel = 0.0;
			car->_accelCmd = myc->accel;
			car->_brakeCmd = brake*cerror;
		}
		else {
			if (myc->getSpeedSqr() < myPathfinder->Seg(myc->getCurrentSegId())->getSpeedsqr()) {
				if (myc->accel < myc->ACCELLIMIT) {
					myc->accel += myc->ACCELINC;
				}
				car->_accelCmd = myc->accel / cerror;
			}
			else {
				if (myc->accel > 0.0) {
					myc->accel -= myc->ACCELINC;
				}
				car->_accelCmd = myc->accel = MIN(myc->accel / cerror, shiftaccel / cerror);
			}
			tdble slipspeed = myc->querySlipSpeed(car);
			if (slipspeed > myc->TCL_SLIP) {
				car->_accelCmd = car->_accelCmd - MIN(car->_accelCmd, (slipspeed - myc->TCL_SLIP) / myc->TCL_RANGE);
			}
		}
	}


	/* check if we are stuck, try to get unstuck */
	tdble bx = myc->getDir()->x, by = myc->getDir()->y;
	tdble cx = myc->currentseg->getMiddle()->x - car->_pos_X, cy = myc->currentseg->getMiddle()->y - car->_pos_Y;
	tdble parallel = (cx*bx + cy*by) / (sqrt(cx*cx + cy*cy)*sqrt(bx*bx + by*by));

	if ((myc->getSpeed() < myc->TURNSPEED) && (parallel < cos(90.0*PI / 180.0)) && (dist2D(myc->getCurrentPos(), myPathfinder->Seg(myc->getCurrentSegId())->getLoc()) > myc->TURNTOL)) {
		myc->turnaround += situation->deltaTime;
	}
	else myc->turnaround = 0.0;
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
		}
		else {
			myc->tr_mode = 2;
			if (parallel < cos(90.0*PI / 180.0) && (dist2D(myc->getCurrentPos(), myPathfinder->Seg(myc->getCurrentSegId())->getLoc()) > myc->TURNTOL)) {
				angle = queryAngleToTrack(car);
				car->_steerCmd = (-angle > 0.0) ? 1.0 : -1.0;
				car->_brakeCmd = 0.0;

				if (myc->accel < 1.0) {
					myc->accel += myc->ACCELINC;
				}
				car->_accelCmd = myc->accel;
				tdble slipspeed = myc->querySlipSpeed(car);
				if (slipspeed < -myc->TCL_SLIP) {
					car->_accelCmd = car->_accelCmd - MIN(car->_accelCmd, (myc->TCL_SLIP - slipspeed) / myc->TCL_RANGE);
				}
			}
			else {
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
}

/* pitstop callback */
static int pitcmd(int index, tCarElt* car, tSituation *s)
{
	PCarDesc* myc = mycar[index - 1];

	car->_pitFuel = MAX(MIN((car->_remainingLaps + 1.0)*myc->fuelperlap - car->_fuel, car->_tank - car->_fuel), 0.0);
	myc->lastpitfuel = MAX(car->_pitFuel, 0.0);
	car->_pitRepair = car->_dammage;
	myc->loadBehaviour(myc->START);
	myc->startmode = true;
	myc->trtime = 0.0;

	return ROB_PIT_IM; /* return immediately */
}

