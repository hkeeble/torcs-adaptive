#include "../trackinc.h"
#include "track.h"
#include "taTrack.h"

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

    static int   envIndex;
    static int	 curSegIndex = 0;
    static int   curStep = 0;
    static tdble radius = 0;
    static tdble totLength = 0;

    void TaAddSegment(taSeg seg, tTrack* taTrack, tTrackSeg* start, tTrackSeg* end, int ext)
    {
        int		        j;
        tdble	        radiusend = 0, dradius;
        tdble	        innerradius;
        tdble	        arc;
        tdble	        length;
        tTrackSeg	    *curSeg;
        tTrackSeg	    *root;
        tdble	        alf;
        tdble	        xr, yr, newxr, newyr;
        tdble	        xl, yl, newxl, newyl;
        tdble	        cenx, ceny;
        tdble	        width, wi2;
        tdble	        x1, x2, y1, y2;
        tdble	        al, alfl;
        tdble	        zsl, zsr, zel, zer, zs, ze;
        tdble	        bankings, bankinge, dz; //  dzl, dzr;
        tdble	        etgt, stgt;
        tdble	        etgtl, stgtl;
        tdble	        etgtr, stgtr;
        tdble	        stepslg = 0;
        int		        steps;
        tTrackSurface   *surface;
        char	        *segName;
        int		        type;
        const char	    *profil;

        tdble	        tl, dtl, T1l, T2l;
        tdble	        tr, dtr, T1r, T2r;
        tdble	        curzel, curzer, curArc, curLength, curzsl, curzsr;
        tdble	        grade;

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

        // Main track width
		width = taTrack->width;
		wi2 = width / 2.0;

        grade = -100000.0;
		root = taTrack->seg; // Root is first segment of track

		if (start == NULL)
		{
			xr = xl = 0.0;
			yr = 0.0;
			yl = width;
			alf = 0.0;
			zsl = zsr = zel = zer = zs = ze = 0.0;
			stgt = etgt = 0.0;
			stgtl = etgtl = 0.0;
			stgtr = etgtr = 0.0;
		}
		else
		{
			int segtype = seg.type;
			if (seg.type == TR_STR)      { } // Do Nothing
			else if (seg.type == TR_LFT) { } // Do Nothing
			else if (seg.type == TR_RGT)
			{
				xr = start->vertex[TR_SR].x;
				yr = start->vertex[TR_SR].y;
				zsl = zsr = zel = zer = zs = ze = start->vertex[TR_SR].z;
				alf = start->angle[TR_ZS];
				xl = xr - width * sin(alf);
				yl = yr + width * cos(alf);
				stgt = etgt = 0.0;
				stgtl = etgtl = 0.0;
				stgtr = etgtr = 0.0;
			}
		}
		
        /* Main Track */
		const char* material = GfParmGetStr(trHandle, TRK_SECT_MAIN, TRK_ATT_SURF, TRK_VAL_ASPHALT);
        surface = AddTrackSurface(trHandle, taTrack, material);
        envIndex = 0;

        InitSides(trHandle, taTrack);

        curSegIndex = 0;

        // --- START OF OLD LOOP ---
		zsl = zel;
		zsr = zer;
		TSTZ(zsl);
		TSTZ(zsr);

		/* Turn Marks */
		if (ext) {
			const char *marks = GfParmGetCurStr(trHandle, path, TRK_ATT_MARKS, NULL);
			ind = 0;
			if (marks) {
				char* tmpmarks = strdup(marks);
				char *s = strtok(tmpmarks, ";");
				while ((s != NULL) && (ind < MAX_TMP_INTS)) {
					mi[ind] = (int)strtol(s, NULL, 0);
					ind++;
					s = strtok(NULL, ";");
				}
				free(tmpmarks);
			}
		}

		/* surface change */
		material = GfParmGetCurStr(trHandle, path, TRK_ATT_SURF, material);
		surface = AddTrackSurface(trHandle, taTrack, material);
		envIndex = (int)GfParmGetCurNum(trHandle, path, TRK_ATT_ENVIND, (char*)NULL, (float) (envIndex+1)) - 1;

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
			// radiusend = GfParmGetCurNum(TrackHandle, path, TRK_ATT_RADIUSEND, (char*)NULL, radius); ??????????
			arc = seg.arc;
			length = (radius + radiusend) / 2.0 * arc;
			seg.length = length;
            break;
		}

        // Name Segment
		std::string sName = "ID" + seg.id;
		segName = new char[strlen(sName.c_str())];
		strcpy(segName, sName.c_str());

		/* elevation and banking (add these to seg?? should def. to 0 */
		/*
		zsl = 
		zsr =
		zel = 
		zer =
		ze = zs = -100000.0;
		ze = 
		zs =
		grade = 
		*/

        // Additional Banking and gradient calculations
        /*
		if (zs != -100000.0) {
			zsr = zsl = zs;
		} else {
			zs = (zsl + zsr) / 2.0;
		}
		if (ze != -100000.0) {
			zer = zel = ze;
		} else if (grade != -100000.0) {
			ze = zs + length * grade;
		} else {
			ze = (zel + zer) / 2.0;
		}
		bankings = atan2(zsl - zsr, width);
		bankinge = atan2(zel - zer, width);
		bankings = GfParmGetCurNum(TrackHandle, path, TRK_ATT_BKS, (char*)NULL, bankings);
		bankinge = GfParmGetCurNum(TrackHandle, path, TRK_ATT_BKE, (char*)NULL, bankinge);
		dz = tan(bankings) * width / 2.0;
		zsl = zs + dz;
		zsr = zs - dz;
		dz = tan(bankinge) * width / 2.0;
		zel = ze + dz;
		zer = ze - dz;
        */

        // Clamps
		TSTZ(zsl);
		TSTZ(zsr);

		/* Get segment profil */
		profil = GfParmGetCurStr(TrackHandle, path, TRK_ATT_PROFIL, TRK_VAL_SPLINE);
		stgtl = etgtl;
		stgtr = etgtr;
		if (strcmp(profil, TRK_VAL_SPLINE) == 0) {
			steps = (int)GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFSTEPS, (char*)NULL, 1.0);
			if (steps == 1) {
				stepslg = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFSTEPSLEN, (char*)NULL, GlobalStepLen);
			if (stepslg) {
				steps = (int)(length / stepslg) + 1;
			} else {
				steps = 1;
			}
			}
			stgtl = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFTGTSL, (char*)NULL, stgtl);
			etgtl = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFTGTEL, (char*)NULL, etgtl);
			stgtr = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFTGTSR, (char*)NULL, stgtr);
			etgtr = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFTGTER, (char*)NULL, etgtr);
			stgt = etgt = -100000.0;
			stgt = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFTGTS, (char*)NULL, stgt);
			etgt = GfParmGetCurNum(TrackHandle, path, TRK_ATT_PROFTGTE, (char*)NULL, etgt);
			if (stgt != -100000.0) {
				stgtl = stgtr = stgt;
			}
			if (etgt != -100000.0) {
				etgtl = etgtr = etgt;
			}
		} else {
			steps = 1;
			stgtl = etgtl = (zel - zsl) / length;
			stgtr = etgtr = (zer - zsr) / length;
		}
		GfParmSetCurNum(TrackHandle, path, TRK_ATT_ID, (char*)NULL, (tdble)curindex);

		//dzl = zel - zsl;
		//dzr = zer - zsr;
		T1l = stgtl * length;
		T2l = etgtl * length;
		tl = 0.0;
		dtl = 1.0 / (tdble)steps;
		T1r = stgtr * length;
		T2r = etgtr * length;
		tr = 0.0;
		dtr = 1.0 / (tdble)steps;

		curStep = 0;
		curzel = zsl;
		curzer = zsr;
		curArc = arc / (tdble)steps;
		curLength = length / (tdble)steps;
		dradius = (radiusend - radius) / (tdble)steps;
		if (radiusend != radius) {
			/* Compute the correct curLength... */
			if (steps != 1) {
			dradius = (radiusend - radius) / (tdble)(steps - 1);
			tdble tmpAngle = 0;
			tdble tmpRadius = radius;
			for (curStep = 0; curStep < steps; curStep++) {
				tmpAngle += curLength / tmpRadius;
				tmpRadius += dradius;
			}
			curLength *= arc / tmpAngle;
			}
		}
		curStep = 0;

		while (curStep < steps) {

			tl += dtl;
			tr += dtr;

			curzsl = curzel;
			curzel = TrackSpline(zsl, zel, T1l, T2l, tl);

			curzsr = curzer;
			curzer = TrackSpline(zsr, zer, T1r, T2r, tr);

			if (dradius != 0) {
				curArc = curLength / radius;
			}

			/* allocate a new segment */
			curSeg = (tTrackSeg*)calloc(1, sizeof(tTrackSeg));
			if (root == NULL) {
				root = curSeg;
				curSeg->next = curSeg;
				curSeg->prev = curSeg;
			} else {
				curSeg->next = root->next;
				curSeg->next->prev = curSeg;
				curSeg->prev = root;
				root->next = curSeg;
				root = curSeg;
			}
			curSeg->type2 = TR_MAIN;
			curSeg->name = segName;
			curSeg->id = curindex;
			curSeg->width = curSeg->startWidth = curSeg->endWidth = width;
			curSeg->surface = surface;
			curSeg->envIndex = envIndex;
			curSeg->lgfromstart = totLength;

			if (ext && ind) {
				int	*mrks = (int*)calloc(ind, sizeof(int));
				tSegExt	*segExt = (tSegExt*)calloc(1, sizeof(tSegExt));

				memcpy(mrks, mi, ind*sizeof(int));
				segExt->nbMarks = ind;
				segExt->marks = mrks;
				curSeg->ext = segExt;
				ind = 0;
			}


			switch (type) {
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
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, width);

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
			curSeg->radiusr = radius + wi2;
			curSeg->radiusl = radius - wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = radius - wi2; /* left side aligned */
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
			newxr = cenx + (innerradius + width) * sin(alf);   /* location of end */
			newyr = ceny - (innerradius + width) * cos(alf);

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

			curSeg->angle[TR_YR] = atan2(curSeg->vertex[TR_ER].z - curSeg->vertex[TR_SR].z, curArc * (innerradius + width));
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curArc * innerradius);
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]) * (innerradius + width);
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curArc;
			curSeg->Kyl = 0;

			/* to find the boundary */
			al = (curSeg->angle[TR_ZE] - curSeg->angle[TR_ZS])/36.0;
			alfl = curSeg->angle[TR_ZS];

			for (j = 0; j < 36; j++) {
				alfl += al;
				x1 = curSeg->center.x + (innerradius) * sin(alfl);   /* location of end */
				y1 = curSeg->center.y - (innerradius) * cos(alfl);
				x2 = curSeg->center.x + (innerradius + width) * sin(alfl);   /* location of end */
				y2 = curSeg->center.y - (innerradius + width) * cos(alfl);
				TSTX(x1); TSTX(x2);
				TSTY(y1); TSTY(y2);
			}

			break;

			case TR_RGT:
			/* right curve */
			curSeg->type = TR_RGT;
			curSeg->radius = radius;
			curSeg->radiusr = radius - wi2;
			curSeg->radiusl = radius + wi2;
			curSeg->arc = curArc;
			curSeg->length = curLength;

			innerradius = radius - wi2; /* right side aligned */
			cenx = xr + innerradius * sin(alf);  /* compute center location */
			ceny = yr - innerradius * cos(alf);
			curSeg->center.x = cenx;
			curSeg->center.y = ceny;

			curSeg->angle[TR_ZS] = alf;
			curSeg->angle[TR_CS] = alf + PI / 2.0;
			alf -= curSeg->arc;
			curSeg->angle[TR_ZE] = alf;

			newxl = cenx - (innerradius + width) * sin(alf);   /* location of end */
			newyl = ceny + (innerradius + width) * cos(alf);
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
			curSeg->angle[TR_YL] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_SL].z, curArc * (innerradius + width));
			curSeg->angle[TR_XS] = atan2(curSeg->vertex[TR_SL].z - curSeg->vertex[TR_SR].z, width);
			curSeg->angle[TR_XE] = atan2(curSeg->vertex[TR_EL].z - curSeg->vertex[TR_ER].z, width);

			curSeg->Kzl = tan(curSeg->angle[TR_YR]) * innerradius;
			curSeg->Kzw = (curSeg->angle[TR_XE] - curSeg->angle[TR_XS]) / curArc;
			curSeg->Kyl = 0;

			/* to find the boundaries */
			al = (curSeg->angle[TR_ZE] - curSeg->angle[TR_ZS])/36.0;
			alfl = curSeg->angle[TR_ZS];

			for (j = 0; j < 36; j++) {
				alfl += al;
				x1 = curSeg->center.x - (innerradius + width) * sin(alfl);   /* location of end */
				y1 = curSeg->center.y + (innerradius + width) * cos(alfl);
				x2 = curSeg->center.x - innerradius * sin(alfl);   /* location of end */
				y2 = curSeg->center.y + innerradius * cos(alfl);
				TSTX(x1); TSTX(x2);
				TSTY(y1); TSTY(y2);
			}
			break;

			}

			AddSides(curSeg, TrackHandle, theTrack, curStep, steps);

			totLength += curSeg->length;
			xr = newxr;
			yr = newyr;
			xl = newxl;
			yl = newyl;
			curindex++;
			curStep++;
			if (type != TR_STR) {
	/* 		printf("radius = %f arc = %f steps %d, length %f, stepslg %f\n", radius, RAD2DEG(curArc), steps, length, curLength); */
			radius += dradius;
			}
		}

    } while (GfParmListSeekNext(TrackHandle, path) == 0);
}