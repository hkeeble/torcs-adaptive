#include "torcsAdaptive.h"
#include "torcsAdaptive\taSeg.h"
#include "raceman.h"

#define taOut(out) printf(strcat("ta >> ", out))

namespace torcsAdaptive
{
	void AddSegment(tRmInfo* ReInfo, const taSeg& segment)
	{
		taOut("Adding new Segment....\n");

		EntityDesc* desc = ReInfo->_reTrackItf.taGetTrackDesc();
		char* acName = ReInfo->_reTrackItf.taGetACName();

		// Add Segment
		taOut("\tAdding segment to track.\n");
		ReInfo->_reTrackItf.taAddSegment(segment, ReInfo->track);

		// Generate new 3D Description
		taOut("\tUpdating 3D Description.\n");
		UpdateACFile(ReInfo->_reTrackItf.taGetTrackState(), ReInfo->track, ReInfo->_reTrackItf.taGetACName());

		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		ReInfo->_reGraphicItf.taUpdate3DDesc(desc, acName);
	}
}