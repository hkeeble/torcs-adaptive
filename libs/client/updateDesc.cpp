#include "torcsAdaptive.h"
#include <zlib.h>

namespace torcsAdaptive
{
	void UpdateACFile(tTrack* track, const char* fName)
	{
		gzFile currentFile = gzopen(fName, "rb");

		if(currentFile == NULL)
			GfFatal("Error! Could not load current track AC file '%900s'.", fName);
	}
}