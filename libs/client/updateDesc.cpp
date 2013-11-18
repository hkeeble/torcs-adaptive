#include "torcsAdaptive.h"
#include <zlib.h>

namespace torcsAdaptive
{
	void UpdateACFile(taTrackState state, tTrack* track, const char* fName)
	{
		FILE* currentFile = fopen(fName, "a");

		if(currentFile == NULL)
			GfFatal("Error! Could not load current track AC file '%900s'.", fName);

		state.segsSinceLastUpdate = 0;
	}
}