#include <track.h>
#include <portability.h>
#include "trackbuild.h"

/*
* Function
*	GetTrackHeader
*
* Description
*	Get the header of the track file
*	in order to know the number of segments
* Parameters
*
*
* Return
*
*
* Remarks
*
*/
void
GetTrackHeader(void *TrackHandle, tTrack* track)
{
	tTrackGraphicInfo *graphic;
	const char **env;
	int i;
	const int BUFSIZE = 256;
	char buf[BUFSIZE];
	char *s;

	track->name = GfParmGetStr(TrackHandle, TRK_SECT_HDR, TRK_ATT_NAME, "no name");
	track->version = (int)GfParmGetNum(TrackHandle, TRK_SECT_HDR, TRK_ATT_VERSION, (char*)NULL, 0);
	track->width = GfParmGetNum(TrackHandle, TRK_SECT_MAIN, TRK_ATT_WIDTH, (char*)NULL, 15.0);
	track->author = GfParmGetStr(TrackHandle, TRK_SECT_HDR, TRK_ATT_AUTHOR, "none");
	track->category = GfParmGetStr(TrackHandle, TRK_SECT_HDR, TRK_ATT_CAT, "road");

	/* Graphic part */
	graphic = &track->graphic;

	graphic->background = GfParmGetStr(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BKGRND,
		"background.png");
	graphic->bgtype = (int)GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGTYPE, (char*)NULL, 0.0);
	/*     if (graphic->bgtype > 2) { */
	/* 	graphic->background2 = GfParmGetStr(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BKGRND2, */
	/* 					    "background.png"); */
	/*     } */
	graphic->bgColor[0] = (float)GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_R, (char*)NULL, 0.0f);
	graphic->bgColor[1] = (float)GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_G, (char*)NULL, 0.0f);
	graphic->bgColor[2] = (float)GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_B, (char*)NULL, 0.1f);

	/* env map images */
	snprintf(buf, BUFSIZE, "%s/%s", TRK_SECT_GRAPH, TRK_LST_ENV);
	graphic->envnb = GfParmGetEltNb(TrackHandle, buf);
	if (graphic->envnb < 1) {
		graphic->envnb = 1;
	}

	graphic->env = (const char**)calloc(graphic->envnb, sizeof(char*));
	env = graphic->env;
	for (i = 1; i <= graphic->envnb; i++) {
		snprintf(buf, BUFSIZE, "%s/%s/%d", TRK_SECT_GRAPH, TRK_LST_ENV, i);
		*env = GfParmGetStr(TrackHandle, buf, TRK_ATT_ENVNAME, "env.png");
		env++;
	}

	track->nseg = 0;

	s = strrchr(track->filename, '/');
	if (s == NULL) {
		s = track->filename;
	}
	else {
		s++;
	}

	track->internalname = _strdup(s);
	s = strrchr(track->internalname, '.');
	if (s != NULL) {
		*s = 0;
	}

	graphic->turnMarksInfo.height = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_HEIGHT, NULL, 1);
	graphic->turnMarksInfo.width = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_WIDTH, NULL, 1);
	graphic->turnMarksInfo.vSpace = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_VSPACE, NULL, 0);
	graphic->turnMarksInfo.hSpace = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_HSPACE, NULL, 0);
}