/**
 *	@file   PTrack.h
 *	@author Henri Keeble
 *	@brief  Declares a class used to represent a procedural track in TORCS. Encapsulates all data required to manage and update a procedural track.
*/

#ifndef _P_TRACK_H_
#define _P_TRACK_H_

#include <fstream>
#include <vector>
#include <string>

#include "PTrackState.h"
#include "PDefs.h"
#include "CarData.h"
#include "PSSGState.h"
#include "track.h"
#include "trackbuild.h"
#include "../../../windows/include/plib/ssg.h"

// Forward declarations (inclusion of track would result in circular inclusion)
struct trackSeg;

using std::string;

namespace procedural
{
#define GR_NAME_TAG "name"
#define GR_SEG_NAME "TKMN"

	/*!
	 * Represents the lifetime of a procedural track.
	 * This class contains all of the functions and members necessary to store and manage a procedural track.
	*/
	class PTrack
	{
	private:
		string configPath;		/*!< The filepath within which all of the track's configuration files are contained.	   */
		string acPath;			/*!< The filepath within which the track's AC file is located.							   */
		string configName;		/*!< The name of the track's XML configuration file.									   */
		string acName;			/*!< The name of the track's AC file.													   */
		string tempACName;		/*!< Name of the temporary AC file used to store new segments.							   */
		PSSGState* ssgState;	/*!< The current state of the scene graph, used to append when adding new segments.		   */
		trackSeg *start, *end;	/*!< Pointers to the current start and end segments of the track.						   */
		tdble totalLength;		/*!< Total length of the track. When the car reaches this distance, the race will finish.  */
		bool hasFinishLine;		/*!< Whether or not a segment with a finish line surface has been added to the track.      */

		/* Internal copying function */
		void cpy(const PTrack& param);

		/** Concatenates two char and returns the result. Used for many of the accessors. */
		const char *const StrCon(const char *const a, const char *const b);

		//! Appends to the track's AC file with data stored in it's temporary AC file.
		 /*!
			\param segmentID The ID of the segment to be appended.
		 */
		void AppendACFile(int segmentID);

		//! Builds a TORCS track strcuture, function dependent upon track library
		/*!
			\param fName The filename to use to build the track.
		*/
		tTrack* BuildTrack(const char* const fName);

	public:

		//! Initializes a procedural track with a configuration. All given paths are relative to the executable.
		/*! 
			\param totalLength	 The total length the track will be. Once the track reaches this length no more segments will be generated.
			\param configpath	 The path in which the configuration XML file can be found.
			\param acpath		 The path in which the AC file can be found.
			\param configname	 The name of the configuration XML file.
			\param loaderoptions The loader options used by the track when appending to the graphical structure.
		*/
		PTrack(tdble totalLength, string configpath, string acpath, string configname, string acname, ssgLoaderOptions* loaderoptions);

		//! Initializes a procedural track with a preloaded set of procedural segments. All given paths are relative to the executable.
		 /*! 
			\param segs			The collection of segments that construct the track.
			\param totalLength	The total length of the track.
			\param configpath	The path in which the configuration XML file can be found.
			\param acpath		The path within which the AC file can be found.
			\param configname	The name of the configuration file being used.
			\param acname		The name of the AC file to be used.
		*/
		PTrack(std::vector<PSeg> segs, tdble totalLength, string configpath, string acpath, string configname, string acname, ssgLoaderOptions* loaderoptions);

		~PTrack();
		PTrack(const PTrack& param);
		PTrack& operator=(const PTrack& param);

		PTrackState state; /*!< The current track state */
		
		trackSeg* root; /*!< Root of the track (the last added segment) */
		
		tTrack* trk; /*!< The TORCS track structure */

		//! Retrieve a pointer to a segment. 
		 /*!
			\param id The ID of the segment to retrieve.
		*/
		tTrackSeg* GetSeg(int id);

		/** Retrieves the total intended length of the track. */
		tdble TotalLength() const;

		/** Updates the track's AC file. */
		void UpdateACFile();

		//!Adds a segment to the track structure. This function does not generate a 3D description for the new segment.
		 /*!
			\param seg		The segment to append to the track.
			\param isFinish The segment is the last segment in the track, and requires a finish line surface. Defaults to false.
		 */
		void AddSegment(const PSeg& seg, bool isFinish = false);

		//! Adds a finish line to the track. A segment with a finish line surface. This function will only work once, a track may only have one finish line.
		void AddFinishLine();

		/** Retrieve the loader options. */
		const ssgLoaderOptions *const GetLoaderOptions();	  

		const string& GetACName();			  /*!< Retrieve the name of the track's AC file. */
		const string& GetACPath();			  /*!< Retrieve the path of the track's AC file. */
		const string& GetTempACName();		  /*!< Retrieve the track's temporary AC file name. */
		const string& GetTempACPath();		  /*!< Retrieve the path of the track's temporary AC file. */
		const string& GetConfigName();		  /*!< Retrieve the track's XML configuration file name. */
		const string& GetConfigPath();		  /*!< Retrieve the path of the track's configuration file. */
		const string& GetPlotName();		  /*!< Retrieve the name of the plot file. */
		const string& GetPlotPath();		  /*!< Retrieve the path of the plot file. */
		const string GetTempACPathAndName();  /*!< Retrieve the track's temporary AC path appended with the temporary AC file name. */
		const string GetACPathAndName();	  /*!< Retrieve the path of the track's AC file appended with the AC file's name. */
		const string GetConfigPathAndName();  /*!< Retrieve the track's XML configuration file path appended with the XML configuration's file name. */
		const string GetPlotPathAndName();	  /*!< Retrieve the track's plot file path and file name. */

		PSSGState*	  GetSSGState();		  /*!< Retrieve the track's current SSG state. */
		EntityDesc*	  GetTrackDesc() const;   /*!< Retrieve the track's current 3D description. */
		trackSeg*	  GetStart() const;		  /*!< Retrieve a pointer to the start segment. */
		trackSeg*	  GetEnd() const;		  /*!< Retrieve a pointer to the current end segment. */


		void SetStart(trackSeg* start); /*!< Set the start segment of the track. */
		void SetEnd(trackSeg* end); /*!< Set the end pointer of the track. */

		/* Debugging Functions */
#ifdef _DEBUG
		void PrintSegsInOrder();   /*!< Prints the segments in order. */
		void PrintSegsBackwards(); /*!< Prints the segments backwards. */
#endif // _DEBUG
	};
}

#endif _P_TRACK_H