/*
	File: PPTrackSegment.h
	Original Author: Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Modified version of TrackSegment from the TORCS robot berniw for a procedural track.
*/

#include "PTrackSegment.h"

namespace procBot
{
	PTrackSegment::PTrackSegment()
	{
		// nothing so far
	}

	void PTrackSegment::init(int id, const tTrackSeg* s, const v3d* lp, const v3d* mp, const v3d* rp)
	{
		/* pointer to the corresponding segment */
		pTrackSeg = (tTrackSeg*)s;

		/* right, middle and left segment (road) border, pointer to right side */
		l = *lp; m = *mp; r = *rp;
		r.dirVector(&l, &tr);
		tr.normalize();

		/* fill in the remaining properties */
		type = s->type;
		raceType = s->raceInfo;
		if (type != TR_STR) radius = s->radius; else radius = FLT_MAX;

		if (s->type == TR_LFT) {
			if (s->lside != NULL && s->lside->style == TR_CURB) l = l - 1.5*tr;
			//else if ((s->lside->style == TR_PLAN) && (strcmp(s->lside->surface->material, TRK_VAL_CONCRETE) == 0)) l = l - 3.0*tr;
		}
		if (s->type == TR_RGT) {
			if (s->rside != NULL && s->rside->style == TR_CURB) r = r + 1.5*tr;
			//else if ((s->rside->style == TR_PLAN) && (strcmp(s->rside->surface->material, TRK_VAL_CONCRETE) == 0)) r = r + 3.0*tr;
		}

		width = distToLeft3D(&r);

		double dz = getRightBorder()->z - getLeftBorder()->z;
		double d = getWidth();
		if (type == TR_LFT) {
			if (dz > 0.0) {
				kalpha = 1.0;
			}
			else {
				kalpha = cos(asin(fabs(dz / d)));
			}
		}
		else if (type == TR_RGT) {
			if (dz < 0.0) {
				kalpha = 1.0;
			}
			else {
				kalpha = cos(asin(fabs(dz / d)));
			}
		}
		else {
			kalpha = 1.0;
		}
	}

	PTrackSegment::~PTrackSegment()
	{
		// nothing so far
	}
}