/*
	File: PTrackFileManager.h
	Author: Henri Keeble
	Desc: Declarations for a class designed to read and write track XML files for the procedural library.
*/

#ifndef _P_TRACK_FILE_MANAGER_
#define _P_TRACK_FILE_MANAGER_

#include "PTrackConfig.h"

namespace procedural
{
	class PTrackFileManager
	{
	public:
		PTrackFileManager() { }
		~PTrackFileManager() { }
		
		/* Writes a track to the given track handle, with the given configuration. */
		void WriteTrackTo(void* newTrackHandle, void* configHandle, tTrack* trk, std::string trkName);
		
		/* Reads a track from the given handle and returns the results. */
		void ReadTrackFrom(void* trackHandle);
	private:
		/* Writes all configuration data given to the given track handle. */
		void WriteConfigDataTo(void* newTrackHandle, const PTrackConfig& config);
		
		/* Writes the given track's segment data to the given track handle. */
		void WriteSegmentDataTo(void* newTrackHandle, tTrack* track);

		/* Writes all Graphic data to the given handle from the given config */
		void WriteGraphicConfigTo(void* newTrackHandle, const PTrackConfig& config);

		/* Writes all main track configuration data to the given handle */
		void WriteMainTrackConfigTo(void* newTrackHandle, const PTrackConfig& config);

		/* Writes out all terrain generation configuration to the given handle */
		void WriteTerrainGenerationConfigTo(void* newTrackHandle, const PTrackConfig& config);

		/* Writes all header data from the given configuration to the given handle */
		void WriteHeaderDataTo(void* newTrackHandle, const PTrackConfig& config);

		/* Writes all data from the given segment into the segment list of the given handle */
		void WriteSegmentTo(void* newTrackHandle, tTrackSeg* seg);
	};
}

#endif // _P_TRACK_FILE_MANAGER_