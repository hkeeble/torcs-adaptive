#include "../trackinc.h"
#include "track.h"
#include "proceduralTrack.h"
#include <sstream>

namespace procedural
{
    static tdble	xmin, xmax, ymin, ymax, zmin, zmax;

#define TSTX(x)			\
    if (xmin > (x)) xmin = (x);	\
    if (xmax < (x)) xmax = (x);

#define TSTY(y)			\
    if (ymin > (y)) ymin = (y);	\
    if (ymax < (y)) ymax = (y);

#define TSTZ(z)			\
    if (zmin > (z)) zmin = (z);	\
    if (zmax < (z)) zmax = (z);

	void PAddSegment(PSeg seg, PTrack* trInfo)
    {
        int		        j;
		tTrackSeg	    *curSeg;
		tdble			radiusend = 0, dradius;
        tdble	        innerradius;
        tdble	        arc;
        tdble	        length;
        tdble	        newxr, newyr, newxl, newyl;
        tdble	        cenx, ceny;
        tdble	        x1, x2, y1, y2;
        tdble	        al, alfl;
        tdble	        zsl, zsr, zel, zer, zs, ze;
        tdble	        etgt, stgt;
        tdble	        etgtl, stgtl;
        tdble	        etgtr, stgtr;

        tdble	        tl, dtl, T1l, T2l;
        tdble	        tr, dtr, T1r, T2r;
        tdble	        curzel, curzer, curArc, curLength, curzsl, curzsr;

		// Get Pointer to current track cache
		tTrack* track = trInfo->trackCache;

		void* trHandle = track->params; // Obtain Track Handle

		// Initialize Buffer
        const int BUFSIZE = 256;
        char	path[BUFSIZE];
        #define MAX_TMP_INTS	256
        int		mi[MAX_TMP_INTS];
        int		ind = 0;

        // Initialize Variables
        arc = length = newxr = newyr = 0;
        zel = zer = etgtl = etgtr = newxl = newyl = 0;

		if (track->seg == NULL) // If Segment Is Start
		{
			TrackState.xr = TrackState.xl = 0.0;
			TrackState.yr = 0.0;
			TrackState.yl = track->width;
			TrackState.alf = 0.0;
			zsl = zsr = zel = zer = zs = ze = 0.0;
			stgt = etgt = 0.0;
			stgtl = etgtl = 0.0;
			stgtr = etgtr = 0.0;
		}
		
		zsl = zel;
		zsr = zer;
		TSTZ(zsl);
		TSTZ(zsr);

		if (seg.type == TR_STR)
		{
			length = seg.length;
			TrackState.radius = radiusend = 0;
		}
		else if (seg.type == TR_LFT || seg.type == TR_RGT)
		{
			TrackState.radius = seg.radius;
			radiusend = seg.radius;
			arc = seg.arc;
			length = (TrackState.radius + radiusend) / 2.0 * arc;
		}

		stgtl = etgtl;
		stgtr = etgtr;

		stgtl = etgtl = (zel - zsl) / length;
		stgtr = etgtr = (zer - zsr) / length;

		GfParmSetCurNum(trHandle, path, TRK_ATT_ID, (char*)NULL, (tdble)TrackState.curSegIndex);

		T1l = stgtl * length;
		T2l = etgtl * length;
		tl = 0.0;
		dtl = 1.0;
		T1r = stgtr * length;
		T2r = etgtr * length;
		tr = 0.0;
		dtr = 1.0;

		curzel = zsl;
		curzer = zsr;
		curArc = arc;
		curLength = length;
		dradius = (radiusend - TrackState.radius);

		// Start Of Step Loop
		tl += dtl;
		tr += dtr;

		curzsl = curzel;
		curzel = TrackSpline(zsl, zel, T1l, T2l, tl);

		curzsr = curzer;
		curzer = TrackSpline(zsr, zer, T1r, T2r, tr);

		if (dradius != 0)
			curArc = curLength / TrackState.radius;

		/* allocate a new segment */
		curSeg = new tTrackSeg();
		if (proceduralTrack->root == NULL)
		{
			proceduralTrack->root = curSeg;
			curSeg->next = curSeg;
			curSeg->prev = curSeg;
		}
		else
		{
			curSeg->next = proceduralTrack->root->next;
			curSeg->next->prev = curSeg;
			curSeg->prev = proceduralTrack->root;
			proceduralTrack->root->next = curSeg;
			proceduralTrack->root = curSeg;
		}

		// Name Segment
		std::string str = "ID" + std::to_string(seg.id);
		curSeg->name = new const char[strlen(str.c_str())];
		strcpy((char*)curSeg->name, str.c_str());

		// Assign Types
		curSeg->type = seg.type;
		curSeg->type2 = DEFAULT_SEG_TYPE2;

		// Assign ID
		curSeg->id = TrackState.curSegIndex;

		// Assign Width
		curSeg->width = curSeg->startWidth = curSeg->endWidth = track->width;
		TrackState.wi2 = track->width / 2.0f;

		// Assign Surface
		curSeg->surface = new tTrackSurface();
		curSeg->surface = AddTrackSurface(trHandle, track, "asphalt-lines");

		curSeg->envIndex = TrackState.envIndex;

		// Assign Length From Start
		curSeg->lgfromstart = TrackState.totLength;

		// Calculate Vertices
		switch (curSeg->type)
		{
			case TR_STR:
			/* straight */
			curSeg->length = curLength;

			newxr = TrackState.xr + curLength * cos(TrackState.alf);      /* find end coordinates */
			newyr = TrackState.yr + curLength * sin(TrackState.alf);
			newxl = TrackState.xl + curLength * cos(TrackState.alf);
			newyl = TrackState.yl + curLength * sin(TrackState.alf);

			curSeg->vertex[TR_SR].x = TrackState.xr;
			curSeg->vertex[TR_SR].y = TrackState.yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = TrackState.xl;
			curSeg->vertex[TR_SL].y = TrackState.yl;
			curSeg->vertex[TR_SL].z = curzsl;

			curSeg->vertex[TR_ER].x = newxr;
			curSeg->vertex[TR_ER].y = newyr;
			curSeg->vertex[TR_ER].z = curzer;

			curSeg->vertex[TR_EL].x = newxl;
			curSeg->vertex[TR_EL].y = newyl;
			curSeg->vertex[TR_EL].z = curzel;

			curSeg->angle[TR_ZS] = TrackState.alf;
			curSeg->angle[TR_ZE] = TrackState.alf;
			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curLength);
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curLength);
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, track->width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, track->width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]);
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curLength;
			curSeg->Kyl = 0;

			curSeg->rgtSideNormal.x = -sin(TrackState.alf);
			curSeg->rgtSideNormal.y = cos(TrackState.alf);

			TSTX(newxr); TSTX(newxl);
			TSTY(newyr); TSTY(newyl);

			break;

			case TR_LFT:
			/* left curve */
			curSeg->radius = TrackState.radius;
			curSeg->radiusr = TrackState.radius + TrackState.wi2;
			curSeg->radiusl = TrackState.radius - TrackState.wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = TrackState.radius - TrackState.wi2; /* left side aligned */
			cenx = TrackState.xl - innerradius * sin(TrackState.alf);  /* compute center location: */
			ceny = TrackState.yl + innerradius * cos(TrackState.alf);
			curSeg->center.x = cenx;
			curSeg->center.y = ceny;

			curSeg->angle[TR_ZS] = TrackState.alf;
			curSeg->angle[TR_CS] = TrackState.alf - PI / 2.0;
			TrackState.alf += curArc;
			curSeg->angle[TR_ZE] = TrackState.alf;

			newxl = cenx + innerradius * sin(TrackState.alf);   /* location of end */
			newyl = ceny - innerradius * cos(TrackState.alf);
			newxr = cenx + (innerradius + track->width) * sin(TrackState.alf);   /* location of end */
			newyr = ceny - (innerradius + track->width) * cos(TrackState.alf);

			curSeg->vertex[TR_SR].x = TrackState.xr;
			curSeg->vertex[TR_SR].y = TrackState.yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = TrackState.xl;
			curSeg->vertex[TR_SL].y = TrackState.yl;
			curSeg->vertex[TR_SL].z = curzsl;

			curSeg->vertex[TR_ER].x = newxr;
			curSeg->vertex[TR_ER].y = newyr;
			curSeg->vertex[TR_ER].z = curzer;

			curSeg->vertex[TR_EL].x = newxl;
			curSeg->vertex[TR_EL].y = newyl;
			curSeg->vertex[TR_EL].z = curzel;

			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curArc * (innerradius + track->width));
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curArc * innerradius);
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, track->width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, track->width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]) * (innerradius + track->width);
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curArc;
			curSeg->Kyl = 0;

			/* to find the boundary */
			al = (curSeg->angle[TR_ZE] - curSeg->angle[TR_ZS])/36.0;
			alfl = curSeg->angle[TR_ZS];

			for (j = 0; j < 36; j++)
			{
				alfl += al;
				x1 = curSeg->center.x + (innerradius) * sin(alfl);   /* location of end */
				y1 = curSeg->center.y - (innerradius) * cos(alfl);
				x2 = curSeg->center.x + (innerradius + track->width) * sin(alfl);   /* location of end */
				y2 = curSeg->center.y - (innerradius + track->width) * cos(alfl);
				TSTX(x1); TSTX(x2);
				TSTY(y1); TSTY(y2);
			}
			break;

			case TR_RGT:
			/* right curve */
			curSeg->radius = TrackState.radius;
			curSeg->radiusr = TrackState.radius - TrackState.wi2;
			curSeg->radiusl = TrackState.radius + TrackState.wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = TrackState.radius - TrackState.wi2; /* right side aligned */
			cenx = TrackState.xr + innerradius * sin(TrackState.alf);  /* compute center location */
			ceny = TrackState.yr - innerradius * cos(TrackState.alf);
			curSeg->center.x = cenx;
			curSeg->center.y = ceny;

			curSeg->angle[TR_ZS] = TrackState.alf;
			curSeg->angle[TR_CS] = TrackState.alf + PI / 2.0;
			TrackState.alf -= curSeg->arc;
			curSeg->angle[TR_ZE] = TrackState.alf;

			newxl = cenx - (innerradius + track->width) * sin(TrackState.alf);   /* location of end */
			newyl = ceny + (innerradius + track->width) * cos(TrackState.alf);
			newxr = cenx - innerradius * sin(TrackState.alf);   /* location of end */
			newyr = ceny + innerradius * cos(TrackState.alf);

			curSeg->vertex[TR_SR].x = TrackState.xr;
			curSeg->vertex[TR_SR].y = TrackState.yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = TrackState.xl;
			curSeg->vertex[TR_SL].y = TrackState.yl;
			curSeg->vertex[TR_SL].z = curzsl;

			curSeg->vertex[TR_ER].x = newxr;
			curSeg->vertex[TR_ER].y = newyr;
			curSeg->vertex[TR_ER].z = curzer;

			curSeg->vertex[TR_EL].x = newxl;
			curSeg->vertex[TR_EL].y = newyl;
			curSeg->vertex[TR_EL].z = curzel;

			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curArc * innerradius);
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curArc * (innerradius + track->width));
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, track->width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, track->width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]) * innerradius;
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curArc;
			curSeg->Kyl = 0;

			/* to find the boundaries */
			al = (curSeg->angle[TR_ZE] - curSeg->angle[TR_ZS])/36.0;
			alfl = curSeg->angle[TR_ZS];

			for (j = 0; j < 36; j++)
			{
				alfl += al;
				x1 = curSeg->center.x - (innerradius + track->width) * sin(alfl);   /* location of end */
				y1 = curSeg->center.y + (innerradius + track->width) * cos(alfl);
				x2 = curSeg->center.x - innerradius * sin(alfl);   /* location of end */
				y2 = curSeg->center.y + innerradius * cos(alfl);
				TSTX(x1); TSTX(x2);
				TSTY(y1); TSTY(y2);
			}
			break;
		}

		// Add Sides
		AddSides(curSeg, trHandle, track, 0, 1);

		// Update Track State
		if (curSeg->type != TR_STR)
			TrackState.radius += dradius;

		proceduralTrack->root = curSeg;

		TrackState.totLength += curSeg->length;
		TrackState.curSegIndex++;
		TrackState.xr = newxr;
		TrackState.yr = newyr;
		TrackState.xl = newxl;
		TrackState.yl = newyl;
		TrackState.segsSinceLastUpdate++;

		if (track->seg == nullptr)
			proceduralTrack->SetStart(curSeg);
		proceduralTrack->SetEnd(curSeg);

		// Update Track with new segment
		track->seg = proceduralTrack->root;
		track->length = TrackState.totLength;
		track->nseg++;

		// Update the Segment Collection
		trInfo->segs.AddSegment(track->seg);
	}
}