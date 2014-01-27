/*
	File: ProceduralTrackManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class to manage a procedural track.
*/
#ifndef _PROC_TRACK_MANAGER_H_
#define _PROC_TRACK_MANAGER_H_

#include "procedural\pTrack.h"
#include "raceman.h"

using namespace procedural;

namespace torcsAdaptive
{
	class ProcecudralTrackManager
		{
		private:
			/* Private Constructor */
			ProcecudralTrackManager(tCarElt* car);

			/* Singleton instance  */
			static ProcecudralTrackManager* instance;

			tRmInfo* raceManager;
			pTrack* track;
			pSegFactory* segFactory;
			pCarData carData;

			/* Calculate percentage of a segment left until the end */
			tdble PercentDistanceToEnd(tdble segLength, tdble distToStart);

		public:
			static ProcecudralTrackManager* Get();
			virtual ~ProcecudralTrackManager();

			/* Initialize car data to be tracked for procedural generation. */
			void Init(tCarElt* car, tRmInfo* RaceManager, pTrack* Track, pSegFactory* SegFactory);

			/* Add a given segment to the track contained in the race manager passed in. */
			void AddSegment(tRmInfo* ReInfo, const pSeg& segment);

			/* Update the AC File of a track contained within the race manaegr passed in. Also requires procedural track information. */
			void UpdateACFile(tRmInfo* ReInfo, pTrack* trInfo);

			/* Update a procedural track, managing it's cache and adding segments if neccesary. */
			void UpdateTrack(tRmInfo* ReInfo);
		};
}

#endif // _PROC_TRACK_MANAGER_H_