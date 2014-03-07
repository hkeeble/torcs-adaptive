/*
	File: PFileManager.cpp
	Author: Henri Keeble
	Desc: Defines the file management class used by the procedural library.
*/

#include "PFileManager.h"

namespace procedural
{
	PFileManager* PFileManager::instance = nullptr;

	std::string PFileManager::ConstructSegmentOutput(tTrackSeg* seg)
	{
		std::string string = "";

		string += std::to_string(seg->id); // Ouput ID
		string += "\n";
		string += std::to_string(seg->type); // Output Type
		string += "\n";
		string += std::to_string(seg->length); // Output Length
		string += "\n";
		string += std::to_string(seg->radius); // Output Radius
		string += "\n";
		string += std::to_string(seg->arc); // Output arc
		string += "\n";

		return string;
	}

	void PFileManager::ConstructSegmentInput(const std::string& input)
	{
		// First, divide the input into lines
		std::vector<std::string> lines = std::vector<std::string>();
		lines.push_back("");
		int curLine = 0;

		for (int i = 0; i < input.length(); i++)
		{
			if (input[i] == '\n')
			{
				lines.push_back("");
				curLine++;
			}
			else
				lines[curLine].append((const char*)input[i]);
		}

		// Then, get the values from the lines
		int id = strToInt(lines[ID_LINE_IDX]);
		int type = strToInt(lines[TYPE_LINE_IDX]);
		tdble radius = strToInt(lines[RADIUS_LINE_IDX]);
		tdble length = strToInt(lines[LENGTH_LINE_IDX]);
		tdble arc = strToInt(lines[ARC_LINE_IDX]);

		// Finally, create the segment with the segment factory and add it the track
		switch (type)
		{
		case TR_STR:
			trackManager->AddSegment(PSegFactory::GetInstance()->CreateSegStr(id, length));
			break;
		case TR_LFT:
			trackManager->AddSegment(PSegFactory::GetInstance()->CreateSegCnr(id, PCornerType::CTLeft, radius, arc));
			break;
		case TR_RGT:
			trackManager->AddSegment(PSegFactory::GetInstance()->CreateSegCnr(id, PCornerType::CTRight, radius, arc));
			break;
		default:
			pOut("Error reading track segment, segmenti is of unknown type or type has been read incorrectly!\n");
			break;
		}
	}

	int PFileManager::strToInt(std::string string)
	{
		std::stringstream ss = std::stringstream(string);
		int ret = 0;
		if (!(ss >> ret))
			pOut("Error, string to int conversion failed!\n");
		return ret;
	}

	PFileManager* PFileManager::Get()
	{
		if (instance == nullptr)
			instance = new PFileManager();
		return instance;
	}

	void PFileManager::OutputTrack(std::string fileName, PTrackManager* trkMngr)
	{
		// Obtain pointers
		trackManager = trkMngr;
		procTrack = trackManager->GetTrack();

		// Create Stream and open file
		std::ofstream stream;
		stream.open(fileName + TRACK_OUT_FTYPE, std::ios::app);

		if (stream.is_open())
		{
			// Get track pointer
			tTrackSeg* seg = procTrack->GetStart();

			// Write Data
			do{
				std::string string = ConstructSegmentOutput(seg);
				stream.write(string.c_str(), string.length());
				stream.write("\n", 1);
				seg = seg->next;
			} while (seg != procTrack->GetStart());

			// Close Stream
			stream.flush();
			stream.close();
		}
		else
		{
			pOut("Failed to output track file, could not open file.");
		}
	}

	void PFileManager::ReadTrack(std::string fileName, PTrackManager* trkMngr)
	{
		// Obtain pointers
		trackManager = trkMngr;
		procTrack = trackManager->GetTrack();

		// Create stream and open file
		std::fstream stream;
		stream.open(fileName, std::ios::in);

		if (stream.is_open())
		{
			// Initialize segments vector
			std::vector<std::string> segments = std::vector<std::string>();
			segments.push_back(""); // Add an initial segment
			int lCount = 0;
			int segNumb = 0;

			// Read each segment into an individual string
			while (!stream.eof())
			{
				char* c = nullptr;
				stream.readsome(c, 1);
				if (*c == '\n')
					lCount++;
				if (lCount > PARAM_COUNT)
				{
					segNumb++;
					segments.push_back("");
				}
				else
					segments[segNumb].append(c);
			}

			// Construct all segments
			for (auto i : segments)
				ConstructSegmentInput(i);
		}
		else
			pOut("Error, could not load file for track reading!\n");
	}
}