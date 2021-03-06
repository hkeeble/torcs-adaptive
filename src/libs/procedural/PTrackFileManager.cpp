/*
	File: PTrackFileManager.h
	Author: Henri Keeble
	Desc: Defines a class designed to read and write track XML files for the procedural library.
*/

#include "PTrackFileManager.h"
#include "portability.h"

namespace procedural
{
	void PTrackFileManager::WriteTrackTo(std::string filePath, std::string trkName, PTrack* track)
	{
		// Read a handle
		void* handle = GfParmReadFile(filePath.c_str(), GFPARM_RMODE_CREAT);

		// Write header information
		GfParmSetStr(handle, P_TRK_SECT_HDR, P_TRK_ATT_NAME, trkName.c_str());
		GfParmSetNum(handle, P_TRK_SECT_HDR, P_TRK_ATT_LENGTH, (char*)nullptr, track->TotalLength());
		GfParmSetNum(handle, P_TRK_SECT_HDR, P_TRK_ATT_SEG_COUNT, (char*)nullptr, track->trk->nseg);
		GfParmSetStr(handle, P_TRK_SECT_HDR, P_TRK_ATT_DESC, "This XML file was generated by the procedural library in TORCS Adaptive.\n More information about TORCS Adaptive can be found on its public GitHub repository: https://github.com/hkeeble/torcs-adaptive");

		// Write Segments
		WriteSegmentDataTo(handle, track->trk);

		// Write out the parameters file
		GfParmWriteFile(filePath.c_str(), handle, std::string(trkName + ".xml").c_str());
	}

	void PTrackFileManager::WriteSegmentDataTo(void* newTrackHandle, tTrack* track)
	{
		tTrackSeg* curSeg = track->seg->next;

		for (int i = 0; i < track->nseg; i++) {
			WriteSegmentTo(newTrackHandle, curSeg);
			curSeg = curSeg->next;
		}
	}

	void PTrackFileManager::WriteSegmentTo(void* newTrackHandle, tTrackSeg* seg)
	{
		const int BUFSIZE = 256;
		char* buf = (char*)malloc(BUFSIZE*sizeof(char));
		snprintf(buf, BUFSIZE, "%s/%s", P_TRK_SECT_SEGS, std::string("seg" + std::to_string(seg->id)).c_str());

		// Write out segment parameters
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_TYPE, (char*)nullptr, seg->type);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_ID, (char*)nullptr, seg->id);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_ARC, (char*)nullptr, seg->arc);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_RADIUS, (char*)nullptr, seg->radius);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_WIDTH, (char*)nullptr, seg->width);
		GfParmSetNum(newTrackHandle, buf, TRK_ATT_LEN, (char*)nullptr, seg->length);

		free(buf);
	}

	std::vector<PSeg> PTrackFileManager::ReadTrackFrom(std::string filePath)
	{
		void* handle = GfParmReadFile(filePath.c_str(), GFPARM_RMODE_STD);
		return ReadSegmentDataFrom(handle);
	}

	std::vector<PSeg> PTrackFileManager::ReadSegmentDataFrom(void* trackHandle)
	{
		int nseg = GfParmGetNum(trackHandle, P_TRK_SECT_HDR, P_TRK_ATT_SEG_COUNT, (char*)nullptr, 0);

		std::vector<PSeg> segs = std::vector<PSeg>();

		// Read segments
		for (int i = 0; i < nseg; i++)
			segs.push_back(ReadSegment(trackHandle, i));

		return segs;
	}

	PSeg PTrackFileManager::ReadSegment(void* trackHandle, int id)
	{
		const int BUFSIZE = 256;
		char* buf = (char*)malloc(BUFSIZE*sizeof(char));
		snprintf(buf, BUFSIZE, "%s/%s", P_TRK_SECT_SEGS, std::string("seg" + std::to_string(id)).c_str());

		// Get segment factory pointer
		PSegFactory* factory = PSegFactory::GetInstance();

		int type = GfParmGetNum(trackHandle, buf, TRK_ATT_TYPE, (char*)nullptr, 1);

		tdble radius = 0.f, arc = 0.f;

		// Create the procedural segment object
		switch (type)
		{
			case TR_STR:
				return factory->CreateSegStr(id, GfParmGetNum(trackHandle, buf, TRK_ATT_LEN, (char*)nullptr, 0));
				break;
			case TR_LFT:
				radius = GfParmGetNum(trackHandle, buf, TRK_ATT_RADIUS, (char*)nullptr, 0);
				arc = GfParmGetNum(trackHandle, buf, TRK_ATT_ARC, (char*)nullptr, 0);
				return factory->CreateSegCnr(id, PCornerType::CTLeft, radius, arc);
				break;
			case TR_RGT:
				radius = GfParmGetNum(trackHandle, buf, TRK_ATT_RADIUS, (char*)nullptr, 0);
				arc = GfParmGetNum(trackHandle, buf, TRK_ATT_ARC, (char*)nullptr, 0);
				return factory->CreateSegCnr(id, PCornerType::CTRight, radius, arc);
				break;
		}
	}

	void PTrackFileManager::SetLoadState(const PTrackLoadState& loadState)
	{
		trkLoadState = loadState;
	}

	const PTrackLoadState& PTrackFileManager::GetLoadState() const
	{
		return trkLoadState;
	}

	void PTrackFileManager::SetLoadStateLength(int length)
	{
		trkLoadState.SetLength(length);
	}
}