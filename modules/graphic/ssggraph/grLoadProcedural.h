/*
	File: grLoadProcedural.h
	Author: Henri Keeble
	Desc: Declarations for functions used to load 3D entities procedurally.
*/
#ifndef _GR_LOAD_PROCEDURAL_H_
#define _GR_LOAD_PROCEDURAL_H_

#include "procedural\PSSGState.h"

namespace procedural
{
	void PSSGAppendEntity(const char* fname, PSSGState* ssgState);
}

#endif  _GR_LOAD_PROCEDURAL_H_