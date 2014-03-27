/**
  * @file 	PTrackLoadState.h
  * @author Henri Keeble
  *	@brief Declarations for a class to keep track of the loading state of a procedural track. That is - whether or not to load a track configuration or a procedural track, and
		   the locations of the XML files required for this process.
*/

#ifndef _P_TRACK_LOAD_STATE_H_
#define _P_TRACK_LOAD_STATE_H_

#include <string>
#include <vector>

#include "pSeg.h"

namespace procedural
{
#define NO_CONFIG_SET " "
#define NO_TRACK_SET " "
#define DFLT_TRK_LEN 1000

	//! Enumeration to represent the type of loading process.
	enum PLoadType {
		TRACK, /*!< Load a pregenerated track. */
		CONFIG /*!< Load a track configuration ready to procedurally generate track segments. */
	};

	/*!	Represents the current load state. A load state is used when initializing a new procedural track. The state contains information such as the configuration to be used, or the file
	    path of a pre-generated track. */
	class PTrackLoadState 
	{
	private:
		PLoadType loadType; /*!< The current load type being used. */
		
		std::string trackName;  /*!< The name of the track to load. */
		std::string configName; /*!< The name of the track configuration to load. */
		std::string trackPath;  /*!< The path of the track files. */
		std::string configPath; /*!< The path of the configuration files to load. */
		
		int trkLength; /*!< Represents the length of the track to load. Used for procedural tracks to track maximum length. */

		//! Internal initialization function. Sets all values to their defaults.
		void init()
		{
			loadType = PLoadType::CONFIG;

			trkLength = DFLT_TRK_LEN;

			trackName = NO_TRACK_SET;
			trackPath = NO_TRACK_SET;
			configName = NO_CONFIG_SET;
			configPath = NO_CONFIG_SET;
		}

	public:
		//! Default constructor, sets all values to defaults.
		PTrackLoadState()
		{
			init();
		}

		//!  Initializes track load data with the intention of loading a pregenerated track.
		/*!
			\param trackName The name of the track to load.
			\param trackPath The path of the track's segment file.
			\param configName The name of the configuration to load.
			\param configPath The file path of the configuration file.
		*/
		PTrackLoadState(std::string trackName, std::string trackPath, std::string configName, std::string configPath)
		{
			init();
			this->trackName = trackName;
			this->configName = configName;
			this->trackPath = trackPath;
			this->configPath = configPath;
			loadType = PLoadType::TRACK;
		}

		//! Initializes track load data with the intention of loading a track configuration.
		/*!
			\param configName The name of the configuration to load.
			\param configPath The path where the configuration file can be found.
		*/
		PTrackLoadState(std::string configName, std::string configPath)
		{
			init();
			this->configPath = configPath;
			this->configName = configName;
			loadType = PLoadType::CONFIG;
		}
		
		//! Sets the current configuration without instantiating a new state object, thus retains track file paths. Does not change load type.
		/*!
			\param configName The name of the configuration to load.
			\param configPath The path where the configuration file can be found.
		*/
		void SetConfiguration(std::string configName, std::string configPath)
		{
			this->configPath = configPath;
			this->configName = configName;
		}

		/* Returns the current load type */
		PLoadType LoadType() { return loadType; }
		
		/** Returns the name of the track to load */
		std::string TrackName() { return trackName; }
		
		/** Returns the path that contains the track's segment file */
		std::string TrackPath() { return trackPath; }
		
		/** Returns the name of the config to load */
		std::string ConfigName() { return configName; }
		
		/** Returns the path that contains the config file */
		std::string ConfigPath() { return configPath; }
		
		/** Returns the track's segment file name */
		std::string TrackFileName() { return trackName + ".xml"; }
		
		/** Returns the configuration file's name */
		std::string ConfigFileName() { return configName + ".xml"; }

		//! Sets the length of the configuration. Used when loading procedural tracks.
		/*!
			\param length The maximum length of the track that will be loaded.
		*/
		void SetLength(int length) { this->trkLength = length; }

		/** Returns the currently configured length */
		int Length() { if (trkLength == 0) trkLength = DFLT_TRK_LEN; return trkLength; }
	};
}

#endif //_P_TRACK_LOAD_STATE_H_