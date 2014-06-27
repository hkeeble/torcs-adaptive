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

	PPathfinder::PPathfinder(tTrack* itrack, tCarElt* icar)
	{
		// Construct a new track description
		track = new PTrackDesc(itrack);
		car = icar;

		// Initialize previous segment count to 0
		previousPSCount = 0;
		
		// Initialize Path Segment Collection
		ps = PathSegCollection(track->segmentCount());

		// Initialize the state manager
		stateMngr = PStateManager(track->GetTorcsTrack());

		// Calculate look ahead
		ahead = track->GetTorcsTrack()->length - getCurrentSegment(car);
	}

	PPathfinder::~PPathfinder()
	{
		if (track)
			delete track;
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

	void PPathfinder::Update(tSituation* situation, PCarDesc* carDesc)
	{
		track->Update();
		stateMngr.Update();

		if (stateMngr.IsUpdateNeeded())
		{
			// Calculate the number of path segments that need to be added
			int nNewSegs = track->segmentCount() - ps.Count();

			// Create an array of path segments equal to the size of the number of new segments needed
			PathSegCollection newSegs = PathSegCollection(nNewSegs);

			// Append the collection of path segments
			ps.Append(newSegs.Segments());

			// Calculate distance to look ahead
			ahead = track->GetTorcsTrack()->length - getCurrentSegment(car);;

			// Compute a new static plan
			Plan(carDesc);
		}
	}

	PathSeg* PPathfinder::Seg(int index)
	{
		return ps(index);
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