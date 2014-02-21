/*
	File: botProc.cpp
	Author: Henri Keeble
	Desc: Based on the 'bt' bot tutorial downloaded from: http://www.berniw.org/tutorials/robot/tutorial.html
	      This bot is used for testing in TORCS-Adaptive.
*/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

static tTrack *curTrack;

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s);
static void newrace(int index, tCarElt* car, tSituation *s);
static void drive(int index, tCarElt* car, tSituation *s);
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt);

#define BOTS 10

static const char* botname[BOTS] = {
	"botproc 1", "botproc 2", "botproc 3", "botproc 4", "botproc 5",
	"botproc 6", "botproc 7", "botproc 8", "botproc 9", "botproc 10"
};

static const char* botdesc[BOTS] = {
	"botproc 1", "botproc 2", "botproc 3", "botproc 4", "botproc 5",
	"botproc 6", "botproc 7", "botproc 8", "botproc 9", "botproc 10"
};

/*
 * Module entry point
 */
extern "C" int botProc(tModInfo *modInfo)
{
	for (int i = 0; i < BOTS; i++) {
		modInfo[i].name = strdup(botname[i]);
		modInfo[i].desc = strdup(botdesc[i]);
		modInfo[i].fctInit = InitFuncPt;
		modInfo[i].gfId = ROB_IDENT;
		modInfo[i].index = i + 1;
	}
	return 0;
}


/* Module interface initialization. */
static int InitFuncPt(int index, void *pt)
{
    tRobotItf *itf  = (tRobotItf *)pt;

    itf->rbNewTrack = initTrack;
    itf->rbNewRace  = newrace;
    itf->rbDrive    = drive;
    itf->rbPitCmd   = nullptr;
    itf->rbEndRace  = endrace;
	itf->rbShutdown = shutdown;
    itf->index      = index;
    return 0;
}


/* Called for every track change or new race. */
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s)
{
    curTrack = track;
    *carParmHandle = nullptr;
}


/* Start a new race. */
static void newrace(int index, tCarElt* car, tSituation *s)
{
}


/* Drive during race. */
static void
drive(int index, tCarElt* car, tSituation *s)
{
    memset(&car->ctrl, 0, sizeof(tCarCtrl));

    float angle;
    const float SC = 1.0;

    angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
    NORM_PI_PI(angle); // put the angle back in the range from -PI to PI
    angle -= SC*car->_trkPos.toMiddle/car->_trkPos.seg->width;

    // set up the values to return
    car->ctrl.steer = angle / car->_steerLock;
    car->ctrl.gear = 1; // first gear
    car->ctrl.accelCmd = 0.6; // 60% accelerator pedal
    car->ctrl.brakeCmd = 0.0; // no brakes
}


/* End of the current race */
static void endrace(int index, tCarElt *car, tSituation *s)
{
}


/* Called before the module is unloaded */
static void shutdown(int index)
{
}

