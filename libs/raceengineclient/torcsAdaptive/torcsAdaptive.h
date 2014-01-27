/*
	File: torcsAdaptive.h
	Author: Henri Keeble
	Desc: Declares an external TAManager object.
*/
#include "torcsAdaptive\TAManager.h"
#ifndef _TORCS_ADAPTIVE_H_
#define _TORCS_ADAPTIVE_H_

#define TA_TR_LENGTH 200

void InitTA();

extern torcsAdaptive::TAManager* taManager;

#endif // _TORCS_ADAPTIVE_H_