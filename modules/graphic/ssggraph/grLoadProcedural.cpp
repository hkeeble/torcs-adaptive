/*
	File: grLoadProcedural.cpp
	Author: Henri Keeble
	Desc: Definitions for functions used to load 3D entities procedurally.
*/

#include <fstream>
#include "grLoadProcedural.h"
#include "grloadac.h"

namespace procedural
{
	static std::fstream infile;

	// Modified version of grssgLoadAC3D
	void PSSGAppendEntity(const char* fname, PSSGState* ssgState)
	{
		// Obtain pointers from ssgState
		ssgLoaderOptions* lopts = ssgState->GetLoaderOptions();
		ssgEntity* currentModel = ssgState->GetDesc();

		if (maxTextureUnits == 0)
			InitMultiTex();

		// Set loader options
		ssgSetCurrentOptions((ssgLoaderOptions*)lopts);
		lopts = ssgGetCurrentOptions();

		// Set model path to filename
		char filename[1024];
		lopts->makeModelPath(filename, fname);

		// Set texture offsets
		sgSetVec2(ssgState->texrep, 1.0, 1.0);
		sgSetVec2(ssgState->texoff, 0.0, 0.0);

		infile.open(filename, std::ios::in);

		if (infile.is_open() == false)
			taOut("Error, infile not open!\n");

		std::string line;
		while (infile.good())
		{
			std::getline(infile, line);
			search(top_tags, (char*)line.c_str());
		}

		delete[] ssgState->current_tfname;
		ssgState->current_tfname = NULL;
		delete[] ssgState->vtab;
		ssgState->vtab = 0;
	}
}