#include "../trackinc.h"
#include "track.h"
#include "torcsAdaptive.h"
#include <sstream>

namespace torcsAdaptive
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

	void TaAddSegment2(taSeg seg, tTrack* taTrack)
	{
		// Used to store vertex positions
		Vec3 startRight;
		Vec3 startLeft;
		Vec3 endRight;
		Vec3 endLeft;

		void* trHandle = taTrack->params; // Obtain Track Handle
		tTrackSeg* curSeg;

		if(taTrack->seg == NULL) // If this is the first segment
		{
			taTrack->nseg = 0;
			taTrack->seg = new tTrackSeg(); // Allocate Memory
			curSeg = taTrack->seg;
			curSeg->next = curSeg;
			curSeg->prev = curSeg;
			curSeg->lgfromstart = 0;

			// Assign Start positions
			startLeft.x = 0;
			startLeft.y = 0;
			startLeft.z = 0;

			startRight.x = startLeft.x + taTrack->width;
			startRight.y = 0;
			startRight.z = 0;
		}
		else
		{
			tdble lengthFromStart = 0;
			tTrackSeg* lastSeg;
			lastSeg = taTrack->seg;

			for(int i = 0; i < taTrack->nseg-1; i++) // Obtain last segment
			{
				lengthFromStart += lastSeg->length;
				lastSeg = lastSeg->next;
			}

			// Allocate memory and assign current segment pointer
			lastSeg->next = NULL; // Override pointer to self
			lastSeg->next = new tTrackSeg(); // Allocate new memory
			curSeg = lastSeg->next; // Assign curseg

			// Close Loop
			lastSeg->next = curSeg;
			curSeg->prev = lastSeg; // Assign prev pointer to last segment
			curSeg->next = taTrack->seg; // Close loop by pointing to Start
			taTrack->seg->prev = curSeg; // Start Previous Pointer to this pointer

			// Assign length from start
			curSeg->lgfromstart = lengthFromStart;

			// Assign Start positions
			startLeft.x = lastSeg->vertex[TR_EL].x;
			startLeft.y = lastSeg->vertex[TR_EL].y;
			startLeft.z = lastSeg->vertex[TR_EL].z;

			startRight.x = lastSeg->vertex[TR_ER].x;
			startRight.y = lastSeg->vertex[TR_ER].y;
			startRight.z = lastSeg->vertex[TR_ER].z;
		}

		// Assign End Positions
		endLeft.x = startLeft.x;
		endLeft.y = startLeft.y;
		endLeft.z = startLeft.z + seg.length;

		endRight.x = startRight.x;
		endRight.y = startRight.y;
		endRight.z = startLeft.z + seg.length;

		// Assign Non-Type specifics
		curSeg->type = seg.type;
		curSeg->id = seg.id;

		// Assign Name
		std::stringstream ssSegName;
		ssSegName << "ID" << seg.id;
		curSeg->name = new const char[strlen(ssSegName.str().c_str())];
		strcpy((char*)curSeg->name, ssSegName.str().c_str());

		// Width
		curSeg->width = taTrack->width;
		curSeg->startWidth = curSeg->width;
		curSeg->endWidth = curSeg->width;

		curSeg->DoVfactor = 0.0f; // DoV Factor (?)

		curSeg->envIndex = 0; // Env Index (?)

		curSeg->height = 1.f; // Max Height of Curbs

		curSeg->ext = NULL; // No Track Extension

		// Assign Segment Styles
		curSeg->style = DEFAULT_SEG_STYLE;
		curSeg->type2 = DEFAULT_SEG_TYPE2;
			
		// Surface
		curSeg->surface = new tTrackSurface(); // Allocate Memory
		*curSeg->surface = taTrack->surfaces[TA_SF_INDEX_ROAD];

		// Corner Details
		curSeg->arc = seg.arc;
		curSeg->radius = seg.radius;
		curSeg->radiusl = seg.radiusl;
		curSeg->radiusr = seg.radiusr;

		// Length
		curSeg->length = seg.length;

		// Vertex Assignment
		switch(curSeg->type)
		{
		case TR_STR:
			curSeg->vertex[TR_SR].x = startRight.x;
			curSeg->vertex[TR_SR].y = startRight.y;
			curSeg->vertex[TR_SR].z = startRight.z;

			curSeg->vertex[TR_SL].x = startLeft.x;
			curSeg->vertex[TR_SL].y = startLeft.y;
			curSeg->vertex[TR_SL].z = startLeft.z;
			
			curSeg->vertex[TR_ER].x = endRight.x;
			curSeg->vertex[TR_ER].y = endRight.y;
			curSeg->vertex[TR_ER].z = endRight.z;
			
			curSeg->vertex[TR_EL].x = endLeft.x;
			curSeg->vertex[TR_EL].y = endLeft.y;
			curSeg->vertex[TR_EL].z = endLeft.z;
			break;
		case TR_RGT || TR_LFT:
			break;
		}

		AddSides(curSeg, trHandle, taTrack, 0, 1);

		trackState->curSegIndex++;
		taTrack->length += curSeg->length;
		taTrack->nseg++;
	}

    void TaAddSegment(taSeg seg, tTrack* taTrack)
    {
        int		        j;
		tTrackSeg	    *curSeg;
		tdble			radiusend = 0, dradius;
        tdble	        innerradius;
        tdble	        arc;
        tdble	        length;
        tdble	        alf;
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

		void* trHandle = taTrack->params; // Obtain Track Handle

		// Initialize Buffer
        const int BUFSIZE = 256;
        char	path[BUFSIZE];
        #define MAX_TMP_INTS	256
        int		mi[MAX_TMP_INTS];
        int		ind = 0;

        // Initialize Variables
        arc = length = alf = newxr = newyr = 0;
        zel = zer = etgtl = etgtr = newxl = newyl = 0;

		if (taTrack->seg == NULL) // If Segment Is Start
		{
			trackState->xr = trackState->xl = 0.0;
			trackState->yr = 0.0;
			trackState->yl = taTrack->width;
			alf = 0.0;
			zsl = zsr = zel = zer = zs = ze = 0.0;
			stgt = etgt = 0.0;
			stgtl = etgtl = 0.0;
			stgtr = etgtr = 0.0;
		}
		
		zsl = zel;
		zsr = zer;
		TSTZ(zsl);
		TSTZ(zsr);

		switch(seg.type)
		{
			case TR_STR:
				length = seg.length;
				trackState->radius = radiusend = 0;
				break;
			case TR_LFT || TR_RGT:
				trackState->radius = seg.radius;
				radiusend = GfParmGetCurNum(trHandle, path, TRK_ATT_RADIUSEND, (char*)NULL, trackState->radius);
				arc = seg.arc;
				length = (trackState->radius + radiusend) / 2.0 * arc;
				seg.length = length;
				break;
		}

		stgtl = etgtl;
		stgtr = etgtr;

		stgtl = etgtl = (zel - zsl) / length;
		stgtr = etgtr = (zer - zsr) / length;

		GfParmSetCurNum(trHandle, path, TRK_ATT_ID, (char*)NULL, (tdble)trackState->curSegIndex);

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
		dradius = (radiusend - trackState->radius);

		// Start Of Step Loop
		tl += dtl;
		tr += dtr;

		curzsl = curzel;
		curzel = TrackSpline(zsl, zel, T1l, T2l, tl);

		curzsr = curzer;
		curzer = TrackSpline(zsr, zer, T1r, T2r, tr);

		if (dradius != 0)
			curArc = curLength / trackState->radius;

		/* allocate a new segment */
		curSeg = new tTrackSeg();
		if (root == NULL)
		{
			root = curSeg;
			curSeg->next = curSeg;
			curSeg->prev = curSeg;
		}
		else
		{
			curSeg->next = root->next;
			curSeg->next->prev = curSeg;
			curSeg->prev = root;
			root->next = curSeg;
			root = curSeg;
		}

		// Name Segment
		std::stringstream ssSegName;
		ssSegName << "ID" << seg.id;
		curSeg->name = new const char[strlen(ssSegName.str().c_str())];
		strcpy((char*)curSeg->name, ssSegName.str().c_str());

		// Assign Types
		curSeg->type = seg.type;
		curSeg->type2 = DEFAULT_SEG_TYPE2;

		// Assign ID
		curSeg->id = trackState->curSegIndex;

		// Assign Width
		curSeg->width = curSeg->startWidth = curSeg->endWidth = taTrack->width;

		// Assign Surface
		curSeg->surface = new tTrackSurface();
		*curSeg->surface = taTrack->surfaces[TA_SF_INDEX_ROAD];

		curSeg->envIndex = trackState->envIndex;

		// Assign Length From Start
		curSeg->lgfromstart = trackState->totLength;

		// Calcualte Vertices
		switch (curSeg->type)
		{
			case TR_STR:
			/* straight */
			curSeg->length = curLength;

			newxr = trackState->xr + curLength * cos(alf);      /* find end coordinates */
			newyr = trackState->yr + curLength * sin(alf);
			newxl = trackState->xl + curLength * cos(alf);
			newyl = trackState->yl + curLength * sin(alf);

			curSeg->vertex[TR_SR].x = trackState->xr;
			curSeg->vertex[TR_SR].y = trackState->yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = trackState->xl;
			curSeg->vertex[TR_SL].y = trackState->yl;
			curSeg->vertex[TR_SL].z = curzsl;

			curSeg->vertex[TR_ER].x = newxr;
			curSeg->vertex[TR_ER].y = newyr;
			curSeg->vertex[TR_ER].z = curzer;

			curSeg->vertex[TR_EL].x = newxl;
			curSeg->vertex[TR_EL].y = newyl;
			curSeg->vertex[TR_EL].z = curzel;

			curSeg->angle[TR_ZS] = alf;
			curSeg->angle[TR_ZE] = alf;
			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curLength);
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curLength);
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, taTrack->width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, taTrack->width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]);
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curLength;
			curSeg->Kyl = 0;

			curSeg->rgtSideNormal.x = -sin(alf);
			curSeg->rgtSideNormal.y = cos(alf);

			TSTX(newxr); TSTX(newxl);
			TSTY(newyr); TSTY(newyl);

			break;

			case TR_LFT:
			/* left curve */
			curSeg->radius = trackState->radius;
			curSeg->radiusr = trackState->radius + trackState->wi2;
			curSeg->radiusl = trackState->radius - trackState->wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = trackState->radius - trackState->wi2; /* left side aligned */
			cenx = trackState->xl - innerradius * sin(alf);  /* compute center location: */
			ceny = trackState->yl + innerradius * cos(alf);
			curSeg->center.x = cenx;
			curSeg->center.y = ceny;

			curSeg->angle[TR_ZS] = alf;
			curSeg->angle[TR_CS] = alf - PI / 2.0;
			alf += curArc;
			curSeg->angle[TR_ZE] = alf;

			newxl = cenx + innerradius * sin(alf);   /* location of end */
			newyl = ceny - innerradius * cos(alf);
			newxr = cenx + (innerradius + taTrack->width) * sin(alf);   /* location of end */
			newyr = ceny - (innerradius + taTrack->width) * cos(alf);

			curSeg->vertex[TR_SR].x = trackState->xr;
			curSeg->vertex[TR_SR].y = trackState->yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = trackState->xl;
			curSeg->vertex[TR_SL].y = trackState->yl;
			curSeg->vertex[TR_SL].z = curzsl;

			curSeg->vertex[TR_ER].x = newxr;
			curSeg->vertex[TR_ER].y = newyr;
			curSeg->vertex[TR_ER].z = curzer;

			curSeg->vertex[TR_EL].x = newxl;
			curSeg->vertex[TR_EL].y = newyl;
			curSeg->vertex[TR_EL].z = curzel;

			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curArc * (innerradius + taTrack->width));
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curArc * innerradius);
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, taTrack->width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, taTrack->width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]) * (innerradius + taTrack->width);
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
				x2 = curSeg->center.x + (innerradius + taTrack->width) * sin(alfl);   /* location of end */
				y2 = curSeg->center.y - (innerradius + taTrack->width) * cos(alfl);
				TSTX(x1); TSTX(x2);
				TSTY(y1); TSTY(y2);
			}
			break;

			case TR_RGT:
			/* right curve */
			curSeg->radius = trackState->radius;
			curSeg->radiusr = trackState->radius - trackState->wi2;
			curSeg->radiusl = trackState->radius + trackState->wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = trackState->radius - trackState->wi2; /* right side aligned */
			cenx = trackState->xr + innerradius * sin(alf);  /* compute center location */
			ceny = trackState->yr - innerradius * cos(alf);
			curSeg->center.x = cenx;
			curSeg->center.y = ceny;

			curSeg->angle[TR_ZS] = alf;
			curSeg->angle[TR_CS] = alf + PI / 2.0;
			alf -= curSeg->arc;
			curSeg->angle[TR_ZE] = alf;

			newxl = cenx - (innerradius + taTrack->width) * sin(alf);   /* location of end */
			newyl = ceny + (innerradius + taTrack->width) * cos(alf);
			newxr = cenx - innerradius * sin(alf);   /* location of end */
			newyr = ceny + innerradius * cos(alf);

			curSeg->vertex[TR_SR].x = trackState-> xr;
			curSeg->vertex[TR_SR].y = trackState->yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = trackState->xl;
			curSeg->vertex[TR_SL].y = trackState->yl;
			curSeg->vertex[TR_SL].z = curzsl;

			curSeg->vertex[TR_ER].x = newxr;
			curSeg->vertex[TR_ER].y = newyr;
			curSeg->vertex[TR_ER].z = curzer;

			curSeg->vertex[TR_EL].x = newxl;
			curSeg->vertex[TR_EL].y = newyl;
			curSeg->vertex[TR_EL].z = curzel;

			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curArc * innerradius);
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curArc * (innerradius + taTrack->width));
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, taTrack->width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, taTrack->width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]) * innerradius;
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curArc;
			curSeg->Kyl = 0;

			/* to find the boundaries */
			al = (curSeg->angle[TR_ZE] - curSeg->angle[TR_ZS])/36.0;
			alfl = curSeg->angle[TR_ZS];

			for (j = 0; j < 36; j++)
			{
				alfl += al;
				x1 = curSeg->center.x - (innerradius + taTrack->width) * sin(alfl);   /* location of end */
				y1 = curSeg->center.y + (innerradius + taTrack->width) * cos(alfl);
				x2 = curSeg->center.x - innerradius * sin(alfl);   /* location of end */
				y2 = curSeg->center.y + innerradius * cos(alfl);
				TSTX(x1); TSTX(x2);
				TSTY(y1); TSTY(y2);
			}
			break;
		}

		// Add Sides
		AddSides(curSeg, trHandle, taTrack, 0, 1);

		// Update Track State
		if (curSeg->type != TR_STR)
			trackState->radius += dradius;

		root = curSeg;
		trackState->totLength += curSeg->length;
		trackState->curSegIndex++;
		trackState->xr = newxr;
		trackState->yr = newyr;
		trackState->xl = newxl;
		trackState->yl = newyl;

		// Update Track with new segment
		taTrack->seg = root;
		taTrack->length = trackState->totLength;
		taTrack->nseg = trackState->curSegIndex;
	}
}