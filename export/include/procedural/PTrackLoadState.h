/*
	File: PTrackLoadState.h
	Author: Henri Keeble
	Desc: Declarations for a class to keep track of the loading state of a procedural track. That is - whether or not to load a track configuration or a procedural track, and
		  the locations of the XML files required for this process.
*/

#ifndef _P_TRACK_LOAD_STATE_H_
#define _P_TRACK_LOAD_STATE_H_

#include <string>
#include <vector>

#include "pSeg.h"

namespace procedural
{
#define NO_CONFIG_SET ""
#define NO_TRACK_SET ""

	/*
	 * PLoadType - Enumeration to represent the type of loading process.
	 * 1 - TRACK - Load a pregenerated track.
	 * 2 - CONFIG - Load a track configuration ready to procedurally generate track segments.
	*/ 
	enum PLoadType {
		TRACK,
		CONFIG
	};

	/* Track Load State */
	class PTrackLoadState {
	private:
		// The current load type
		PLoadType loadType;
		
		std::string trackName;
		std::string configName;
		std::string trackPath;
		std::string configPath;

		// Internal initialization function
		void init()
		{
			loadType = PLoadType::CONFIG;

			trackName = NO_TRACK_SET;
			trackPath = NO_TRACK_SET;
			configName = NO_CONFIG_SET;
			configPath = NO_CONFIG_SET;
		}

	public:
		PTrackLoadState()
		{
			init();
		}

		/*
		 * Initializes track load data with the intention of loading a pregenerated track.
		 * trackName The name of the track to load.
		 * trackPath The path of the track's segment file.
		 * configName The name of the configuration to load.
		 * configPath The file path of the configuration file.
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

		/* 
		 * Initializes track load data with the intention of loading a track configuration.
		 * configName The name of the configuration to load.
		 * configPath The path where the configuration file can be found.
		*/
		PTrackLoadState(std::string configName, std::string configPath)
		{
			init();
			this->configPath = configPath;
			this->configName = configName;
			loadType = PLoadType::CONFIG;
		}
		
		/* 
		 * Sets the current configuration without instantiating a new state object, thus retains track file paths. Does not change load type.
		 * configName The name of the configuration to load.
		 * configPath The path where the configuration file can be found.
		 */
		void SetConfiguration(std::string configName, std::string configPath)
		{
			this->configPath = configPath;
			this->configName = configName;
		}

		/* Returns the current load type */
		PLoadType LoadType() { return loadType; }
		
		/* Returns the name of the track to load */
		std::string TrackName() { return trackName; }
		
		/* Returns the path that contains the track's segment file */
		std::string TrackPath() { return trackPath; }
		
		/* Returns the name of the config to load */
		std::string ConfigName() { return configName; }
		
		/* Returns the path that contains the config file */
		std::string ConfigPath() { return configPath; }
		
		/* Returns the track's segment file name */
		std::string TrackFileName() { return trackName + ".xml"; }
		
		/* Returns the configuration file's name */
		std::string ConfigFileName() { return configName + ".xml"; }
	};
}

#endif //_P_TRACK_LOAD_STATE_H_