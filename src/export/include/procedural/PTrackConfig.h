/*
	File: PTrackConfig.h
	Author: Henri Keeble
	Desc: Declares a class designed to represent a track configuration internally.
*/

#ifndef _P_TRACK_CONFIG_H_
#define _P_TRACK_CONFIG_H_

#include <string>
#include "track.h"

namespace procedural
{

// Defaults, in case of error reading files
#define DFLT_BG_IMG "background.png"
#define DFLT_TR_SURF "grass"
#define DFLT_TRK_SURF "asphalt-lines"
#define DFLT_SIDE_SURF "concrete"
#define DFLT_SIDE_TYPE "level"
#define DFLT_TRK_WIDTH 15.0
#define DFLT_SIDE_WIDTH 3.0
#define DFLT_PROF_STEPSLEN 8
#define DFLT_ORIENTATION "counter-clockwise"

	/* Represents header configuration */
	struct CfgHeader
	{
		CfgHeader();
		CfgHeader(std::string trkName);

		std::string name;
		std::string category;
		int version;
		std::string author;
		std::string description;
	};

	/* Represents a terrain generation configuration */
	struct CfgTerrainGeneration
	{
		void Read(void* handle);

		int trkStep;
		int brdrMargin;
		int brdrStep;
		int brdrHeight;
		std::string surface;
		std::string orientation;
	};

	/* Represents a graphical configuration */
	struct CfgGraphic
	{
		CfgGraphic();
		CfgGraphic(std::string trkName);
		void Read(void* handle);

		// 3D Description file
		std::string descFile;

		// Background image filename
		std::string bgImage;

		// Background Colors
		tdble BGColorR, BGColorG, BGColorB;
		tdble ambBGColorR, ambBGColorG, ambBGColorB;
		tdble diffBGColorR, diffBGColorG, diffBGColorB;
		tdble specBGColorR, specBGColorG, specBGColorB;

		// Light positions
		int lPosX;
		int lPosY;
		int lPosZ;

		// Terrain generation accessor
		CfgTerrainGeneration TerrainGeneration() const { return terrainGeneration; }

		private:
			CfgTerrainGeneration terrainGeneration;
	};

	/* Represents a main track configuration */
	struct CfgMainTrack
	{
		void Read(void* handle);

		std::string surfaceGrp;
		std::string surfaceName;
		int stepsLength;

		std::string rSideSurface;
		std::string lSideSurface;

		tdble width;
		tdble lSideWidth;
		tdble rSideWidth;

		std::string lSideType;
		std::string rSideType;

		int profilStepsLength;
	};

	/* Represents a a configuration for a track internally */
	class PTrackConfig
	{
	private:
		CfgHeader header;
		CfgGraphic graphic;
		CfgMainTrack mainTrack;

	public:
		PTrackConfig(void* fileHandle, std::string trkName);

		CfgHeader Header() const { return header; }
		CfgGraphic Graphic() const { return graphic; }
		CfgMainTrack MainTrack() const { return mainTrack; }
	};
}

#endif // _P_TRACK_CONFIG_H_