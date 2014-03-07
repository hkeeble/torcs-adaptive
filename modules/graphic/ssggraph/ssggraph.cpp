/***************************************************************************

    file                 : ssggraph.cpp
    created              : Thu Aug 17 23:19:19 CEST 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: ssggraph.cpp,v 1.12.2.1 2008/12/31 03:53:56 berniw Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <tgfclient.h>

#include "grmain.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static int
graphInit(int /* idx */, void *pt)
{
    tGraphicItf *itf = (tGraphicItf*)pt;
    
    itf->inittrack     = initTrack;
    itf->initcars      = initCars;
    itf->initview      = initView;
    itf->refresh       = refresh;
    itf->shutdowncars  = shutdownCars;
    itf->shutdowntrack = shutdownTrack;
    //itf->bendcar       = bendCar;

	/* PROCEDURAL INTERFACE */
	itf->PGrInit		 = procedural::PGrInit;
	itf->PGrAppend3DDesc = procedural::PGrAppend3DDesc;
	itf->PGrAttach3DDesc = procedural::PGrAttach3DDesc;
	itf->PGrDetach3DDesc = procedural::PGrDetach3DDesc;

    return 0;
}

/*
 * Function
 *	ssggraph
 *
 * Description
 *	module entry point
 *
 * Parameters
 *	
 *
 * Return
 *	
 */
extern "C" int
ssggraph(tModInfo *modInfo)
{
    modInfo->name = _strdup("ssggraph");		        		/* name of the module (short) */
    modInfo->desc = _strdup("The Graphic Library using PLIB ssg");	/* description of the module (can be long) */
    modInfo->fctInit = graphInit;				/* init function */
    modInfo->gfId = 1;						/* v 1  */

    return 0;
}
