/*
	File: PSSGState.cpp
	Author: Henri Keeble
	Desc: Defines a class to represent the current SSG state. Used to append to an
		  ssgEntity as opposed to loading an entirely new one.
*/

#include "PSSGState.h"

namespace procedural
{
	std::fstream PSSGState::infile;

	PSSGState::PSSGState()
	{
		currentLine = 0;
		desc = nullptr;
		lopts = nullptr;

		current_material = nullptr;
		current_colour = nullptr;
		current_branch = nullptr;
		current_tfname = nullptr;
		current_tbase = nullptr;
		current_ttiled = nullptr;
		current_tskids = nullptr;
		current_tshad = nullptr;
		current_data = nullptr;
		vtab = nullptr;

		num_materials = 0;
	}

	PSSGState::PSSGState(const PSSGState& param)
	{
		if (param.desc)
			desc = param.desc;
		else
			desc = nullptr;

		if (param.lopts)
		{
			lopts = new ssgLoaderOptions();
			*lopts = *param.lopts;
		}
		else
			lopts = nullptr;
	}

	PSSGState& PSSGState::operator= (const PSSGState& param)
	{
		if (&param == this)
			return *this;
		else
		{
			if (param.desc)
				desc = param.desc;
			else
				desc = nullptr;

			if (param.lopts)
			{
				lopts = new ssgLoaderOptions();
				*lopts = *param.lopts;
			}
			else
				lopts = nullptr;

			return *this;
		}
	}

	PSSGState::~PSSGState()
	{
		if (lopts)
			delete lopts;
	}

	ssgEntity* PSSGState::GetDesc() const
	{
		return desc;
	}

	ssgLoaderOptions* PSSGState::GetLoaderOptions() const
	{
		return lopts;
	}

	void PSSGState::SetDesc(ssgEntity* desc)
	{
		this->desc = desc;
	}

	void PSSGState::SetLoaderOptions(ssgLoaderOptions* loaderOptions)
	{
		lopts = loaderOptions;
	}

	void PSSGState::InitFile(const char* fName)
	{
		// Count lines in AC File
		infile.open(fName, std::ios::in);
		
		std::string line;
		if (infile.is_open())
			while (infile.good())
				std::getline(infile, line);
		else
			taOut("Error initializing PSSGState file, file not opened!\n");
	}
}