#include "../trackinc.h"
#include "track.h"
#include "taTrack.h"
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
		curSeg->type2 = DEFAULT_SEG_STYLE2;
			
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

    void TaAddSegment(taSeg seg, tTrack* taTrack, tTrackSeg* start, tTrackSeg* end, int ext)
    {
        int		        j;
		tdble			radius, radiusend = 0, dradius;
        tdble	        innerradius;
        tdble	        arc;
        tdble	        length;
        tTrackSeg	    *curSeg;

        tdble	        alf;
        tdble	        xr, yr, newxr, newyr;
        tdble	        xl, yl, newxl, newyl;
        tdble	        cenx, ceny;
        tdble	        x1, x2, y1, y2;
        tdble	        al, alfl;
        tdble	        zsl, zsr, zel, zer, zs, ze;
        tdble	        bankings, bankinge, dz; //  dzl, dzr;
        tdble	        etgt, stgt;
        tdble	        etgtl, stgtl;
        tdble	        etgtr, stgtr;
        tdble	        stepslg = 0;

        char	        *segName;
        int		        type;
        const char	    *profil;

        tdble	        tl, dtl, T1l, T2l;
        tdble	        tr, dtr, T1r, T2r;
        tdble	        curzel, curzer, curArc, curLength, curzsl, curzsr;

		void* trHandle = taTrack->params;

        const int BUFSIZE = 256;
        char	path[BUFSIZE];
        #define MAX_TMP_INTS	256
        int		mi[MAX_TMP_INTS];
        int		ind = 0;

        // Initialize Variables
        arc = length = alf = xr = yr = newxr = newyr = xl = yl = 0;
        zel = zer = etgtl = etgtr = newxl = newyl = 0;
        type = 0;

		if (start == NULL)
		{
			xr = xl = 0.0;
			yr = 0.0;
			yl = taTrack->width;
			alf = 0.0;
			zsl = zsr = zel = zer = zs = ze = 0.0;
			stgt = etgt = 0.0;
			stgtl = etgtl = 0.0;
			stgtr = etgtr = 0.0;
		}
		else
		{
			int segtype = seg.type;
			if (seg.type == TR_STR)      { } // Do Nothing??
			else if (seg.type == TR_LFT) { } // Do Nothing??
			else if (seg.type == TR_RGT)
			{
				xr = start->vertex[TR_SR].x;
				yr = start->vertex[TR_SR].y;
				zsl = zsr = zel = zer = zs = ze = start->vertex[TR_SR].z;
				alf = start->angle[TR_ZS];
				xl = xr - taTrack->width * sin(alf);
				yl = yr + taTrack->width * cos(alf);
				stgt = etgt = 0.0;
				stgtl = etgtl = 0.0;
				stgtr = etgtr = 0.0;
			}
		}
		
        // --- START OF OLD LOOP ---
		zsl = zel;
		zsr = zer;
		TSTZ(zsl);
		TSTZ(zsr);

		/* Turn Marks (may not be neccesary) */
		if (ext)
		{
			const char *marks = GfParmGetCurStr(trHandle, path, TRK_ATT_MARKS, NULL);
			ind = 0;
			if (marks)
			{
				char* tmpmarks = strdup(marks);
				char *s = strtok(tmpmarks, ";");
				while ((s != NULL) && (ind < MAX_TMP_INTS))
				{
					mi[ind] = (int)strtol(s, NULL, 0);
					ind++;
					s = strtok(NULL, ";");
				}
				free(tmpmarks);
			}
		}

		/* surface change */
		trackState->material = GfParmGetCurStr(trHandle, path, TRK_ATT_SURF, trackState->material);
		trackState->surface = AddTrackSurface(trHandle, taTrack, trackState->material);
		trackState->envIndex = (int)GfParmGetCurNum(trHandle, path, TRK_ATT_ENVIND, (char*)NULL, (float) (trackState->envIndex+1)) - 1;

		/* Segment type and length */
		type = seg.type;

		switch(type)
		{
			case TR_STR:
				length = seg.length;
				radius = radiusend = 0;
				break;
			case TR_LFT || TR_RGT:
				radius = seg.radius;
				radiusend = GfParmGetCurNum(trHandle, path, TRK_ATT_RADIUSEND, (char*)NULL, radius);
				arc = seg.arc;
				length = (radius + radiusend) / 2.0 * arc;
				seg.length = length;
				break;
		}

        // Name Segment
		std::stringstream ssSegName;
		ssSegName << "ID" << seg.id;

		/* elevation and banking (add these to seg?? should def. to 0 */
		/*
		zsl = 
		zsr =
		zel = 
		zer =
		ze = 
		zs =
		trackState->grade = 
		*/
		ze = zs = -100000.0;

        // Additional Banking and gradient calculations
		if (zs != -100000.0)
			zsr = zsl = zs;
		else
			zs = (zsl + zsr) / 2.0;
		if (ze != -100000.0)
			zer = zel = ze;
		else if (trackState->grade != -100000.0)
			ze = zs + length * trackState->grade;
		else
			ze = (zel + zer) / 2.0;

		// Calculate Banking
		bankings = atan2(zsl - zsr, taTrack->width);
		bankinge = atan2(zel - zer, taTrack->width);
		bankings = GfParmGetCurNum(trHandle, path, TRK_ATT_BKS, (char*)NULL, bankings);
		bankinge = GfParmGetCurNum(trHandle, path, TRK_ATT_BKE, (char*)NULL, bankinge);
		dz = tan(bankings) * taTrack->width / 2.0;
		zsl = zs + dz;
		zsr = zs - dz;
		dz = tan(bankinge) * taTrack->width / 2.0;
		zel = ze + dz;
		zer = ze - dz;

        // Clamps
		TSTZ(zsl);
		TSTZ(zsr);

		/* 
		**	Get segment profil
		**	Profil appears to influence number of steps. Seems to default to Spline, if not specified within track parameters.
		*/
		profil = GfParmGetCurStr(trHandle, path, TRK_ATT_PROFIL, TRK_VAL_SPLINE);
		stgtl = etgtl;
		stgtr = etgtr;

		stgtl = etgtl = (zel - zsl) / length;
		stgtr = etgtr = (zer - zsr) / length;

		GfParmSetCurNum(trHandle, path, TRK_ATT_ID, (char*)NULL, (tdble)trackState->curSegIndex);

		//dzl = zel - zsl;
		//dzr = zer - zsr;
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
		dradius = (radiusend - radius);

		// START OF OLD STEP LOOP

		tl += dtl;
		tr += dtr;

		curzsl = curzel;
		curzel = TrackSpline(zsl, zel, T1l, T2l, tl);

		curzsr = curzer;
		curzer = TrackSpline(zsr, zer, T1r, T2r, tr);

		if (dradius != 0)
			curArc = curLength / radius;

		/* allocate a new segment */
		curSeg = (tTrackSeg*)calloc(1, sizeof(tTrackSeg));
		if (trackState->root == NULL)
		{
			trackState->root = curSeg;
			curSeg->next = curSeg;
			curSeg->prev = curSeg;
		}
		else
		{
			curSeg->next = trackState->root->next;
			curSeg->next->prev = curSeg;
			curSeg->prev = trackState->root;
			trackState->root->next = curSeg;
			trackState->root = curSeg;
		}

		curSeg->type2 = TR_MAIN;

		curSeg->name = new const char[strlen(ssSegName.str().c_str())];
		strcpy((char*)curSeg->name, ssSegName.str().c_str());

		curSeg->id = trackState->curSegIndex;
		curSeg->width = curSeg->startWidth = curSeg->endWidth = taTrack->width;
		curSeg->surface = trackState->surface;
		curSeg->envIndex = trackState->envIndex;
		curSeg->lgfromstart = trackState->totLength;

		if (ext && ind)
		{
			int	*mrks = (int*)calloc(ind, sizeof(int));
			tSegExt	*segExt = (tSegExt*)calloc(1, sizeof(tSegExt));

			memcpy(mrks, mi, ind*sizeof(int));
			segExt->nbMarks = ind;
			segExt->marks = mrks;
			curSeg->ext = segExt;
			ind = 0;
		}


		switch (type)
		{
			case TR_STR:
			/* straight */
			curSeg->type = TR_STR;
			curSeg->length = curLength;

			newxr = xr + curLength * cos(alf);      /* find end coordinates */
			newyr = yr + curLength * sin(alf);
			newxl = xl + curLength * cos(alf);
			newyl = yl + curLength * sin(alf);

			curSeg->vertex[TR_SR].x = xr;
			curSeg->vertex[TR_SR].y = yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = xl;
			curSeg->vertex[TR_SL].y = yl;
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
			curSeg->type = TR_LFT;
			curSeg->radius = radius;
			curSeg->radiusr = radius + trackState->wi2;
			curSeg->radiusl = radius - trackState->wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = radius - trackState->wi2; /* left side aligned */
			cenx = xl - innerradius * sin(alf);  /* compute center location: */
			ceny = yl + innerradius * cos(alf);
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

			curSeg->vertex[TR_SR].x = xr;
			curSeg->vertex[TR_SR].y = yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = xl;
			curSeg->vertex[TR_SL].y = yl;
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
			curSeg->type = TR_RGT;
			curSeg->radius = radius;
			curSeg->radiusr = radius - trackState->wi2;
			curSeg->radiusl = radius + trackState->wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = radius - trackState->wi2; /* right side aligned */
			cenx = xr + innerradius * sin(alf);  /* compute center location */
			ceny = yr - innerradius * cos(alf);
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

			curSeg->vertex[TR_SR].x = xr;
			curSeg->vertex[TR_SR].y = yr;
			curSeg->vertex[TR_SR].z = curzsr;

			curSeg->vertex[TR_SL].x = xl;
			curSeg->vertex[TR_SL].y = yl;
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

		AddSides(curSeg, trHandle, taTrack, 0, 1);

		trackState->totLength += curSeg->length;

		xr = newxr;
		yr = newyr;
		xl = newxl;
		yl = newyl;

		trackState->curSegIndex++;

		if (type != TR_STR)
			radius += dradius;

		trackState->root = curSeg;

		// Update Track with new segment
		taTrack->seg = trackState->root;
		taTrack->length = trackState->totLength;
		taTrack->nseg = trackState->curSegIndex;
	}
}