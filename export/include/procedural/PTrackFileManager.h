/*
	File: PTrackFileManager.h
	Author: Henri Keeble
	Desc: Declarations for a class designed to read and write track XML files for the procedural library.
*/

#ifndef _P_TRACK_FILE_MANAGER_
#define _P_TRACK_FILE_MANAGER_

#include <fstream>
#include <vector>

#include "PTrackConfig.h"
#include "PTrackLoadState.h"

namespace procedural
{
	#define P_TRK_SECT_SEGS "Segments"
	#define P_TRK_SECT_HDR "Header"

	#define P_TRK_ATT_SEG_COUNT "Number of Segments"
	#define P_TRK_ATT_DESC "Description"
	#define P_TRK_ATT_NAME "Track Name"

	/*
	 * A class that represents a file manager designed to write out track files for the procedural library. This class also holds onto a state for loading tracks.
	 * This state consists of configuration and track file names and paths. This is used in order to determine the configuration to load, and whether or not to
	 * load any pregenerated segments.
	 */
	class PTrackFileManager
	{
	public:
		PTrackFileManager() { trkLoadState = PTrackLoadState(); }
		~PTrackFileManager() { }
		
		/*
		 * Writes a track to the given track file.
		 * filePath The path to write the track to.
		 * trkName  The name of the track being written.
		 * track	The track to write out.
		 */
		void WriteTrackTo(std::string filePath, std::string trkName, tTrack* track);

		/*
		 * Reads track data from the given file, and constructs a vector of procedural segment objects.
		 * filePath The path of the XML segment file to read.
		 */
		std::vector<PSeg> ReadTrackFrom(std::string filePath);

		/*
		 * Sets the current load state for tracks using this file manager.
		 * loadState The load state to use.
		 */
		void SetLoadState(const PTrackLoadState& loadState);

		/*
		 * Retrieves the current track load state stored by this track file manager.
		*/
		const PTrackLoadState& GetLoadState() const;

		/*
		* Sets the length of the current track load state.
		* length The length of the track that will be procedurally generated using the current load state.
		*/
		void SetLoadStateLength(int length);

	private:
		PTrackLoadState trkLoadState; /* The current track load state. */

		/*
		 * Writes the given track's segment data to the given track handle.
		 * newTrackHandle The parameter handle of the new track. All data is written to this handle.
		 * track		  The track to write out segment data for.
		 */
		void WriteSegmentDataTo(void* newTrackHandle, tTrack* track);

		/*
		 * Writes an individual segment into a given handle.
		 * newTrackHandle The handle to write the segment out to.
		 * seg			  The segment to write into the handle.
		 */
		void WriteSegmentTo(void* newTrackHandle, tTrackSeg* seg);

		/*
		 * Reads segments from a given track handle. Returns a collection of procedural segments.
		 * trackHandle The handle to read segment data from.
		 */
		std::vector<PSeg> ReadSegmentDataFrom(void* trackHandle);

		/*
		 * Reads a single segment.
		 * trackHandle The handle to read a segment from.
		 * id		   The ID of the segment to read.
		 */
		PSeg ReadSegment(void* trackHandle, int id);
	};
}

#endif // _P_TRACK_FILE_MANAGER_