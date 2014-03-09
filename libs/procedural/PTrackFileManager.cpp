/*
	File: PTrackFileManager.h
	Author: Henri Keeble
	Desc: Defines a class designed to read and write track XML files for the procedural library.
*/

#include "PTrackFileManager.h"

namespace procedural
{
	void PTrackFileManager::WriteTrackTo(void* newTrackHandle, void* configHandle, tTrack* trk, std::string trkName)
	{
		PTrackConfig config = PTrackConfig(configHandle, trkName);
		WriteConfigDataTo(newTrackHandle, config);
		WriteSegmentDataTo(newTrackHandle, trk);
	}

	void PTrackFileManager::ReadTrackFrom(void* trackHandle)
	{

	}

	void PTrackFileManager::WriteConfigDataTo(void* newTrackHandle, const PTrackConfig& config)
	{
		WriteHeaderDataTo(newTrackHandle, config);
		WriteGraphicConfigTo(newTrackHandle, config);
		WriteMainTrackConfigTo(newTrackHandle, config);
	}

	void PTrackFileManager::WriteSegmentDataTo(void* newTrackHandle, tTrack* track)
	{
		tTrackSeg* end = track->seg;
		tTrackSeg* curSeg = end;

		// Get the starting segment
		do {
			curSeg = curSeg->prev;
		} while (curSeg->prev != end);

		tTrackSeg* start = curSeg;

		// Write out each segment
		do {
			WriteSegmentTo(newTrackHandle, curSeg);
			curSeg = curSeg->next;
		} while (curSeg->next != start);
	}

	void PTrackFileManager::WriteGraphicConfigTo(void* newTrackHandle, const PTrackConfig& config)
	{
		// Image/Description files
		GfParmSetStr(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_3DDESC, config.Graphic().descFile.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_BKGRND, config.Graphic().bgImage.c_str());

		// BG Color
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_R, (char*)nullptr, config.Graphic().BGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_R, (char*)nullptr, config.Graphic().BGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_R, (char*)nullptr, config.Graphic().BGColorR);

		// Ammbient BG Color
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_AMBIENT_R, (char*)nullptr, config.Graphic().ambBGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_AMBIENT_G, (char*)nullptr, config.Graphic().ambBGColorG);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_AMBIENT_B, (char*)nullptr, config.Graphic().ambBGColorB);

		// Diffuse BG Color
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_DIFFUSE_R, (char*)nullptr, config.Graphic().diffBGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_DIFFUSE_G, (char*)nullptr, config.Graphic().diffBGColorG);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_DIFFUSE_B, (char*)nullptr, config.Graphic().diffBGColorB);

		// Specular Color
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_SPEC_R, (char*)nullptr, config.Graphic().specBGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_SPEC_G, (char*)nullptr, config.Graphic().specBGColorG);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_SPEC_B, (char*)nullptr, config.Graphic().specBGColorB);

		// Light positions
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_LIPOS_X, (char*)nullptr, config.Graphic().BGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_LIPOS_Y, (char*)nullptr, config.Graphic().BGColorR);
		GfParmSetNum(newTrackHandle, TRK_SECT_GRAPH, TRK_ATT_LIPOS_Z, (char*)nullptr, config.Graphic().BGColorR);
		
		// Write out terrain generation config data
		WriteTerrainGenerationConfigTo(newTrackHandle, config);
	}

	void PTrackFileManager::WriteMainTrackConfigTo(void* newTrackHandle, const PTrackConfig& config)
	{
		// Surfaces
		GfParmSetStr(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_SURF, config.MainTrack().surfaceName.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_RSSURF, config.MainTrack().rSideSurface.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_LSSURF, config.MainTrack().lSideSurface.c_str());

		// Widths
		GfParmSetNum(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_WIDTH, (char*)nullptr, config.MainTrack().width);
		GfParmSetNum(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_LSW, (char*)nullptr, config.MainTrack().rSideWidth);
		GfParmSetNum(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_RSW, (char*)nullptr, config.MainTrack().lSideWidth);

		// Side Types
		GfParmSetStr(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_LST, config.MainTrack().lSideType.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_RST, config.MainTrack().rSideType.c_str());

		// Profile Steps Length
		GfParmSetNum(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_PROFSTEPSLEN, (char*)nullptr, config.MainTrack().stepsLength);

		// Steps Length
		GfParmSetNum(newTrackHandle, TRK_SECT_MAIN, TRK_ATT_STEPLEN, (char*)nullptr, config.MainTrack().stepsLength);
	}

	void PTrackFileManager::WriteTerrainGenerationConfigTo(void* newTrackHandle, const PTrackConfig& config)
	{
		// Terrain Generation Values
		GfParmSetNum(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_TSTEP, (char*)nullptr, config.Graphic().TerrainGeneration().trkStep);
		GfParmSetNum(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_BMARGIN, (char*)nullptr, config.Graphic().TerrainGeneration().brdrMargin);
		GfParmSetNum(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_BSTEP, (char*)nullptr, config.Graphic().TerrainGeneration().brdrStep);
		GfParmSetNum(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_BHEIGHT, (char*)nullptr, config.Graphic().TerrainGeneration().brdrHeight);

		// Terrain Generation Strings
		GfParmSetStr(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_SURF, config.Graphic().TerrainGeneration().surface.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_ORIENTATION, config.Graphic().TerrainGeneration().orientation.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_TERRAIN, TRK_ATT_RELIEF, (config.Header().name + "-relief.ac").c_str());
	}

	void PTrackFileManager::WriteHeaderDataTo(void* newTrackHandle, const PTrackConfig& config)
	{
		GfParmSetStr(newTrackHandle, TRK_SECT_HDR, TRK_ATT_NAME, config.Header().name.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_HDR, TRK_ATT_CAT, config.Header().category.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_HDR, TRK_ATT_AUTHOR, config.Header().author.c_str());
		GfParmSetStr(newTrackHandle, TRK_SECT_HDR, TRK_ATT_DESCR, config.Header().description.c_str());
		GfParmSetNum(newTrackHandle, TRK_SECT_HDR, TRK_ATT_VERSION, (char*)nullptr, config.Header().version);
	}

	void PTrackFileManager::WriteSegmentTo(void* newTrackHandle, tTrackSeg* seg)
	{
		const int BUFFSIZE = 256;
		char* buf = new char[BUFFSIZE];

		_snprintf(buf, BUFFSIZE, "%s/%s", TRK_SECT_MAIN, TRK_LST_SEGMENTS);

		GfParmSetNum(newTrackHandle, buf, TRK_ATT_ID, (char*)nullptr, seg->id);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_ARC, (char*)nullptr, seg->arc);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_RADIUS, (char*)nullptr, seg->radius);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_WIDTH, (char*)nullptr, seg->width);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_LEN, (char*)nullptr, seg->length);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_TYPE, (char*)nullptr, seg->type);

		delete buf;
	}
}