/*
	File: berniProc.xml
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Car description definitions for a procedural robot in torcs adaptive.
*/

#include "PCarDesc.h"
#include "BerniwPathfinding.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

namespace procPathfinder
{
	const double PCarDesc::PATHERR = 0.5;				/* if derror > PATHERR we take actions to come back to the path [m] */
	const double PCarDesc::CORRLEN = 30.0;				/* CORRLEN * derror is the length of the correction [m] */
	const double PCarDesc::TURNTOL = 1.0;				/* tolerance for end backing up [m] */
	const double PCarDesc::TURNSPEED = 3.0;			/* if speed lower than this you can back up [m/s] */
	const double PCarDesc::MARGIN = 0.3;				/* security margin from track border [m] */
	const double PCarDesc::STABLESPEED = 80.0;			/* we brake currentspeed/stablespeed if car seems unstable [m/s] */
	const double PCarDesc::TIMETOCATCH = 3.0;			/* when do we start thinking about overtaking [s]*/
	const double PCarDesc::MINOVERTAKERANGE = 250.0;	/* minimum length for overtaking [m] */
	const double PCarDesc::OVERTAKERADIUS = 100.0;		/* min allowed radius to start overtaking [m] */
	const double PCarDesc::OVERTAKEDIST = 4.0;			/* planning distance of CG's while overtaking [m] */
	const double PCarDesc::OVERTAKEMINDIST = 1.7;		/* minimal distance between cars overtaking [m] */
	const double PCarDesc::OVERTAKEANGLE = 0.03;		/* minimum angle between cars [radians] */
	const double PCarDesc::OVERTAKEMARGIN = 1.7;		/* distance between cars [m] */
	const double PCarDesc::MAXALLOWEDPITCH = 0.06;		/* max allowed angle between road and car [radians] */
	const double PCarDesc::FLYSPEED = 55.0;			/* above this speed anti fly system is activated [m/s] */
	const double PCarDesc::OVERLAPSTARTDIST = 70.0;	/* distance where we start to check the need to let pass the opponent */
	const double PCarDesc::OVERLAPPASSDIST = 30.0;		/* distance smaller than that and waiting long enough -> let him pass */
	const double PCarDesc::OVERLAPWAITTIME = 5.0;		/* minimal waiting time before we consider let him pass */
	const double PCarDesc::LAPBACKTIMEPENALTY = -30.0; /* penalty if i am able to "lap back" [s] */
	const double PCarDesc::TCL_SLIP = 2.0;				/* [m/s] range [0..10] */
	const double PCarDesc::TCL_RANGE = 5.0;			/* [m/s] range [0..10] */
	const double PCarDesc::SHIFT = 0.9;				/* [-] (% of rpmredline) */
	const double PCarDesc::SHIFT_MARGIN = 4.0;			/* [m/s] */
	const double PCarDesc::MAX_SPEED = 84.0;			/* [m/s] */
	const double PCarDesc::MAX_FUEL_PER_METER = 0.0008;/* [liter/m] fuel consumtion */
	const double PCarDesc::LOOKAHEAD_MAX_ERROR = 2.0;	/* [m] */
	const double PCarDesc::LOOKAHEAD_FACTOR = 1.0 / 3.0; /* [-] */

	PCarDesc::PCarDesc(PPathfinder* pathfinder, tCarElt* car, tSituation *situation)
	{
		AEROMAGIC = GfParmGetNum(car->_carHandle, "berniProc private", "caero", (char*)NULL, 1.6f);
		CFRICTION = GfParmGetNum(car->_carHandle, "berniProc private", "cfriction", (char*)NULL, 1.0f);

		/* init pointer to car data */
		setCarPtr(car);
		initCGh();
		initCarGeometry();
		updatePos();
		updateDir();
		updateSpeedSqr();
		updateSpeed();

		/* damage and fuel status */
		lastfuel = GfParmGetNum(car->_carHandle, SECT_CAR, PRM_FUEL, NULL, 100.0);
		undamaged = situation->_maxDammage;
		if (undamaged == 0) undamaged = 10000;
		MAXDAMMAGE = undamaged / 2;
		fuelperlap = 0.0;
		lastpitfuel = 0.0;

		/* set up some car properties */
		wheelbase = car->priv.wheel[FRNT_RGT].relPos.x - car->priv.wheel[REAR_RGT].relPos.x;
		wheeltrack = 2 * fabs(car->priv.wheel[REAR_RGT].relPos.y);

		carmass = GfParmGetNum(car->_carHandle, SECT_CAR, PRM_MASS, NULL, 0.0);
		mass = carmass + lastfuel;

		/* which wheels are driven */
		const char *traintype = GfParmGetStr(car->_carHandle, SECT_DRIVETRAIN, PRM_TYPE, VAL_TRANS_RWD);
		if (strcmp(traintype, VAL_TRANS_RWD) == 0) {
			drivetrain = DRWD;
		}
		else if (strcmp(traintype, VAL_TRANS_FWD) == 0) {
			drivetrain = DFWD;
		}
		else if (strcmp(traintype, VAL_TRANS_4WD) == 0) {
			drivetrain = D4WD;
		}

		updateCa();

		double cx = GfParmGetNum(car->_carHandle, SECT_AERODYNAMICS, PRM_CX, (char*)NULL, 0.0);
		double frontarea = GfParmGetNum(car->_carHandle, SECT_AERODYNAMICS, PRM_FRNTAREA, (char*)NULL, 0.0);
		cw = 0.625*cx*frontarea;

		cgcorr_b = 0.46;

		path = pathfinder;

		// Current and destination segment initialization
		currentsegid = destsegid = path->getCurrentSegment(car);
		currentseg = path->Track()->getSegmentPtr(currentsegid);
		destseg = path->Track()->getSegmentPtr(destsegid);
		currentpathseg = path->Seg(currentsegid);
		destpathseg = path->Seg(destsegid);

		turnaround = 0.0;
		tr_mode = 0;
		accel = 1.0;
		fuelchecked = false;
		startmode = true;
		trtime = 0.0;
		derror = 0.0;

		/*
		DIST; MAXRELAX; MAXANGLE; ACCELINC; SPEEDSQRFACTOR; GCTIME; ACCELLIMIT; PATHERRFACTOR
		*/

		double ba[6][8] = {
			{ 1.2, 0.9, 25.0, 0.1, 1.2, 0.2, 1.0, 5.0 },
			{ 1.2, 0.9, 20.0, 0.1, 1.1, 0.5, 1.0, 5.0 },
			{ 1.2, 0.9, 15.0, 0.1, 1.0, 0.5, 1.0, 5.0 },
			{ 1.3, 0.9, 15.0, 0.02, 0.98, 0.5, 1.0, 5.0 },
			{ 1.6, 0.9, 15.0, 0.01, 0.95, 0.5, 1.0, 5.0 },
			{ 1.2, 0.9, 45.0, 0.1, 1.0, 0.5, 1.0, 1.0 }
		};

		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 8; j++) {
				behaviour[i][j] = ba[i][j];
			}
		}

		loadBehaviour(NORMAL);
	}


	PCarDesc::~PCarDesc()
	{

	}


	/*
	info for debugging
	*/
	void PCarDesc::info(void)
	{
		printf("wheelbase: %f\n", wheelbase);
		printf("wheeltrack: %f\n", wheeltrack);
		for (int i = 0; i < MAX_GEARS; i++) {
			printf("%d\t%f\n", i, getCarPtr()->_gearRatio[i]);
		}
		printf("Offset: %d\n", getCarPtr()->_gearOffset);
		printf("#gears: %d\n", getCarPtr()->_gearNb);
		printf("gear: %d\n", getCarPtr()->_gear);
		printf("steerlock: %f rad, %f�\n", getCarPtr()->_steerLock, getCarPtr()->_steerLock * 180 / PI);
		printf("cgcorr_b: %f\n", cgcorr_b);
		printf("car index: %d\n", getCarPtr()->index);
		printf("race nb: %d\n", getCarPtr()->_raceNumber);
	}


	/*
	updates values needed from driver, pathfinder, etc. the are stored here, that we don't need to compute
	them several times or pass tons of parameters.
	*/
	void PCarDesc::update(PTrackDesc* track, tCarElt* car, tSituation *situation)
	{
		updatePos();
		updateDir();
		updateSpeedSqr();
		updateSpeed();

		/* update currentsegment and destination segment id's */
		int searchrange = MAX((int)ceil(situation->deltaTime*speed + 1.0) * 2, 4);
		currentsegid = destsegid = path->getCurrentSegment(car, searchrange);
		double l = 0.0;

		while (l < 2.0 * wheelbase) {
			l = l + path->Seg(destsegid)->getLength();
			destsegid = (destsegid + 1 + path->getnPathSeg()) % path->getnPathSeg();
		}

		currentseg = track->getSegmentPtr(currentsegid);
		destseg = track->getSegmentPtr(destsegid);
		currentpathseg = path->Seg(currentsegid);
		updateDError(track);
		int error = (int)(MIN(LOOKAHEAD_MAX_ERROR, derror));
		if (error < 0)
			error = 0;
		int lookahead = (destsegid + (int)(error*speed*LOOKAHEAD_FACTOR)) % path->getnPathSeg();
		destpathseg = path->Seg(lookahead);

		mass = carmass + car->priv.fuel;
		trtime += situation->deltaTime;
		deltapitch = MAX(-track->getSegmentPtr(currentsegid)->getKgamma() - me->_pitch, 0.0);
	}


	void PCarDesc::loadBehaviour(int id) {
		bmode = id;
		DIST = behaviour[id][0];
		MAXRELAX = behaviour[id][1];
		MAXANGLE = behaviour[id][2];
		ACCELINC = behaviour[id][3];
		SPEEDSQRFACTOR = behaviour[id][4];
		GCTIME = behaviour[id][5];
		ACCELLIMIT = behaviour[id][6];
		PATHERRFACTOR = behaviour[id][7];
	}


	void PCarDesc::updateCa()
	{
		const char *WheelSect[4] = { SECT_FRNTRGTWHEEL, SECT_FRNTLFTWHEEL, SECT_REARRGTWHEEL, SECT_REARLFTWHEEL };
		double rearwingarea = GfParmGetNum(me->_carHandle, SECT_REARWING, PRM_WINGAREA, (char*)NULL, 0);
		double rearwingangle = GfParmGetNum(me->_carHandle, SECT_REARWING, PRM_WINGANGLE, (char*)NULL, 0);
		double wingca = 1.23*rearwingarea*sin(rearwingangle);
		double cl = GfParmGetNum(me->_carHandle, SECT_AERODYNAMICS, PRM_FCL, (char*)NULL, 0.0) + GfParmGetNum(me->_carHandle, SECT_AERODYNAMICS, PRM_RCL, (char*)NULL, 0.0);
		double h = 0.0;

		for (int i = 0; i < 4; i++) h += GfParmGetNum(me->_carHandle, WheelSect[i], PRM_RIDEHEIGHT, (char*)NULL, 0.20f);
		h *= 1.5; h = h*h; h = h*h; h = 2.0 * exp(-3.0*h);
		ca = AEROMAGIC*(h*cl + 4.0*wingca);
	}


	/*
	compute the slip velocity.
	*/
	double PCarDesc::querySlipSpeed(tCarElt* car)
	{
		double s;
		switch (drivetrain) {
		case DRWD:
			s = (car->_wheelSpinVel(REAR_RGT) +
				car->_wheelSpinVel(REAR_LFT)) * car->_wheelRadius(REAR_LFT) / 2.0;
			break;
		case DFWD:
			s = (car->_wheelSpinVel(FRNT_RGT) +
				car->_wheelSpinVel(FRNT_LFT)) * car->_wheelRadius(FRNT_LFT) / 2.0;
			break;
		case D4WD:
			s = ((car->_wheelSpinVel(REAR_RGT) +
				car->_wheelSpinVel(REAR_LFT)) * car->_wheelRadius(REAR_LFT) +
				(car->_wheelSpinVel(FRNT_RGT) +
				car->_wheelSpinVel(FRNT_LFT)) * car->_wheelRadius(FRNT_LFT)) / 4.0;
			break;
		default:
			s = 0.0;
			break;
		}
		return s - car->_speed_x;
	}


	/*
	compute an acceleration value for a given speed
	*/
	double PCarDesc::queryAcceleration(tCarElt * car, double speed)
	{
		double a, gr = car->_gearRatio[car->_gear + car->_gearOffset], rm = car->_enginerpmRedLine;
		switch (drivetrain) {
		case DRWD:
			a = speed / car->_wheelRadius(REAR_RGT) * gr / rm;
			break;
		case DFWD:
			a = speed / car->_wheelRadius(FRNT_RGT) * gr / rm;
			break;
		case D4WD:
			a = speed / (car->_wheelRadius(REAR_RGT) + car->_wheelRadius(FRNT_RGT)) * 2.0 * gr / rm;
			break;
		default:
			a = 1.0;
			break;
		}
		if (a > 1.0) return 1.0; else return a;
	}

	double PCarDesc::getMaxSpeed()
	{
		// Get required values from car handle
		double maxRPM = GfParmGetNum(me->priv.carHandle, "Engine", "revs maxi", "rpm", 0);
		double gearRatio = GfParmGetNum(me->priv.carHandle, "Gearbox/gears/6", "ratio", (char*)NULL, 0);
		double diffRatio = GfParmGetNum(me->priv.carHandle, "Rear Differential", "ratio", (char*)NULL, 0);
		double wheelDiameter = GfParmGetNum(me->priv.carHandle, "Front Right Wheel", "rim diameter", "in", 0);

		// Compute final gear ratio and wheel circumference in meters
		double finalGear = gearRatio*diffRatio;
		double wheelCircumference = (wheelDiameter * M_PI) / 39.370;

		// Calculate maximum theoretical speed in KM/H
		double maxVel = (maxRPM / 60) * (1 / finalGear) * wheelCircumference;

		return maxVel;
	}

	void POtherCarDesc::init(PTrackDesc* itrack, tCarElt* car, tSituation *situation)
	{
		track = itrack;
		dt = situation->deltaTime;
		setCarPtr(car);
		currentsegid = track->getCurrentSegment(car);

		initCGh();
		updatePos();
		updateDir();
		updateSpeedSqr();
		updateSpeed();
	}


	void POtherCarDesc::update()
	{
		updatePos();
		updateDir();
		updateSpeedSqr();
		updateSpeed();

		int searchrange = MAX((int)ceil(dt*speed + 1.0) * 2, 4);
		currentsegid = track->getCurrentSegment(getCarPtr(), currentsegid, searchrange);
	}

	/* Updates the direction to the desired trajectory */
	void PCarDesc::updateDError(PTrackDesc* trackDesc)
	{
		derror = distToPath(trackDesc, path->Segs(), currentsegid, &currentpos); // Get the distance to the desired path for the current segment
		derrorsgn = (derror >= 0.0) ? 1.0 : -1.0; // Determine which side of the path the car is
		derror = fabs(derror); // Ensure value is absolute
	}

	void PCarDesc::initCarGeometry()
	{
		CARWIDTH = me->_dimension_y;
		CARLEN = me->_dimension_x;
	}
}