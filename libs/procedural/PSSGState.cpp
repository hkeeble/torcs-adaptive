/*
	File: PSSGState.cpp
	Author: Henri Keeble
	Desc: Defines a class to represent the current SSG state. Used to append to an
		  ssgEntity as opposed to loading an entirely new one.
*/

#include "PSSGState.h"

namespace procedural
{
	PSSGState::PSSGState()
	{
		// Initialize all pointers to null
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
		ntab = nullptr;
		t0tab = nullptr;
		t1tab = nullptr;
		t2tab = nullptr;
		t3tab = nullptr;
		vertlist = nullptr;
		striplist = nullptr;

		// Initialize integers
		num_materials = num_kids = last_num_kids = totalnv = totalstripe = 0;
		current_flags = -1;
		
		usenormal = false;
		firsttime = true;
		usestrip = true;

		t_xmax = -999999.0;
		t_ymax = -999999.0;
		t_xmin = +999999.0;
		t_ymin = +999999.0;
	}

	PSSGState::~PSSGState()
	{
		if (current_material)
			delete current_material;
		if (current_colour)
			delete current_colour;
		if (current_branch)
			delete current_branch;
		if (current_tfname)
			delete current_tfname;
		if (current_tbase)
			delete current_tbase;
		if (current_ttiled)
			delete current_ttiled;
		if (current_tskids)
			delete current_tskids;
		if (current_tshad)
			delete current_tshad;
		if (current_data)
			delete current_data;

		ClearTables();
	}

	ssgEntity* PSSGState::GetDesc() const
	{
		return desc;
	}

	ssgLoaderOptions* PSSGState::GetLoaderOptions() const
	{
		return lopts;
	}

	void PSSGState::UpdateDesc()
	{
		desc = current_branch;
	}

	void PSSGState::SetLoaderOptions(ssgLoaderOptions* loaderOptions)
	{
		lopts = loaderOptions;
	}

	void PSSGState::ClearTables()
	{
		if (vtab)
			delete[] vtab;
		if (ntab)
			delete[] ntab;
		if (t0tab)
			delete[] t0tab;
		if (t1tab)
			delete[] t1tab;
		if (t2tab)
			delete[] t2tab;
		if (t3tab)
			delete[] t3tab;
	}

	void PSSGState::InitTables(int nVerts)
	{
		vtab = new sgVec3[nVerts];
		ntab = new sgVec3[nVerts];
		t0tab = new sgVec2[nVerts];
		t1tab = new sgVec2[nVerts];
		t2tab = new sgVec2[nVerts];
		t3tab = new sgVec2[nVerts];
	}
}