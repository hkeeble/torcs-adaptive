/**
 *	@file PTrackFileManager.h
 *  @author Henri Keeble
 *	@brief Declarations for a class designed to read and write track XML files for the procedural library.
*/

#ifndef _P_TRACK_FILE_MANAGER_
#define _P_TRACK_FILE_MANAGER_

#include <fstream>
#include <vector>

#include "pTrack.h"
#include "track.h"
#include "PTrackLoadState.h"

namespace procedural
{
	#define P_TRK_SECT_SEGS "Segments"
	#define P_TRK_SECT_HDR "Header"

	#define P_TRK_ATT_SEG_COUNT "Number of Segments"
	#define P_TRK_ATT_DESC "Description"
	#define P_TRK_ATT_NAME "Track Name"
	#define P_TRK_ATT_LENGTH "Length"
	#define P_TRK_ATT_CAT "Category"

	/*!
	 * A class that represents a file manager designed to write out track files for the procedural library. This class also holds onto a state for loading tracks, inside a PTrackLoadState object.
	 */
	class PTrackFileManager
	{
	public:
		PTrackFileManager() { trkLoadState = PTrackLoadState(); }
		~PTrackFileManager() { }
		
		//! Writes a track to the given track file.
		/*! filePath The path to write the track to.
			\param trkName  The name of the track being written.
			\param track	The track to write out.
			\param category The category of race in which the track was generated.
		 */
		void WriteTrackTo(std::string filePath, std::string trkName, PTrack* track);

		//! Reads track data from the given file, and constructs a vector of procedural segment objects.
		 /*!
			\param filePath The path of the XML segment file to read.
		 */
		std::vector<PSeg> ReadTrackFrom(std::string filePath);

		//! Sets the current load state for tracks using this file manager.
		 /*!
			\param loadState The load state to use.
		 */
		void SetLoadState(const PTrackLoadState& loadState);

		/** Retrieves the current track load state stored by this track file manager. */
		const PTrackLoadState& GetLoadState() const;

		//! Sets the length of the current track load state.
		/*! 
			\param length The length of the track that will be procedurally generated using the current load state.
		*/
		void SetLoadStateLength(int length);

	private:
		PTrackLoadState trkLoadState; /*!< The current track load state. */

		//! Writes the given track's segment data to the given track handle.
		/*!
			\param newTrackHandle The parameter handle of the new track. All data is written to this handle.
			\param track		  The track to write out segment data for.
		*/
		void WriteSegmentDataTo(void* newTrackHandle, tTrack* track);

		//! Writes an individual segment into a given handle.
		 /*!
			\param newTrackHandle The handle to write the segment out to.
			\param seg			  The segment to write into the handle.
		 */
		void WriteSegmentTo(void* newTrackHandle, tTrackSeg* seg);

		//! Reads segments from a given track handle. Returns a collection of procedural segments.
		 /*!
			\param trackHandle The handle to read segment data from.
		 */
		std::vector<PSeg> ReadSegmentDataFrom(void* trackHandle);

		//! Reads a single segment.
		/*!
			\param trackHandle The handle to read a segment from.
			\param id		   The ID of the segment to read.
		*/
		PSeg ReadSegment(void* trackHandle, int id);
	};
}

#endif // _P_TRACK_FILE_MANAGER_