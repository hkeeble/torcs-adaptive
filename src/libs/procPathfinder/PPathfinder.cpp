/*
	File: PPathfinder.cpp
	Original Author:  Bernhard Wymann
	Modified by: Henri Keeble
	Desc: Declarations for a pathfinder that works for procedural tracks, updates planning as new segments are added.
*/

#include "PPathfinder.h"

namespace procPathfinder
{
	const double PPathfinder::TPRES = PI / (NTPARAMS - 1);	/* resolution of the steps */

	PPathfinder::PPathfinder(PTrackDesc* track)
	{
		// Construct a new track description
		this->track = track;

		// Initialize previous segment count to 0
		previousPSCount = 0;
		
		// Initialize Path Segment Collection
		ps = PathSegCollection(track->segmentCount());

		// Initialize the state manager
		stateMngr = PStateManager(track->GetTorcsTrack());

		ahead = 0;
	}

	PPathfinder::~PPathfinder()
	{
		// Nothing yet, resources managed by track module
	}

	void PPathfinder::PlotPath(char* filename)
	{
		FILE* fd = fopen(filename, "w");

		/* plot path */
		for (int i = 0; i < ps.Count(); i++) {
			fprintf(fd, "%f,%f\n", ps(i)->getLoc()->x, ps(i)->getLoc()->y);
		}
		fclose(fd);
	}

	void PPathfinder::Update(PCarDesc* carDesc)
	{
		UpdateTrackDesc(carDesc);
		UpdatePath(carDesc);
	}

	void PPathfinder::UpdateTrackDesc(PCarDesc* carDesc)
	{
		track->Update();

		// Calculate distance to look ahead
		ahead = track->GetTorcsTrack()->length - getCurrentSegment(carDesc->getCarPtr());

	}

	void PPathfinder::UpdatePath(PCarDesc* carDesc)
	{
		stateMngr.Update();
		if (stateMngr.IsUpdateNeeded())
		{
			// Calculate the number of path segments that need to be added
			int nNewSegs = track->segmentCount() - ps.Count();

			// Create an array of path segments equal to the size of the number of new segments needed
			PathSegCollection newSegs = PathSegCollection(nNewSegs);

			// Append the collection of path segments
			ps.Append(newSegs.Segments());

			// Compute a new static plan
			Plan(carDesc);
		}
	}

	PathSeg* PPathfinder::Seg(int index)
	{
		return ps(index);
	}

	const PathSegCollection& PPathfinder::Segs() const
	{
		return ps;
	}

	PTrackDesc* PPathfinder::Track() const
	{
		return track;
	}

	int PPathfinder::LookAhead() const
	{
		return ahead;
	}
}