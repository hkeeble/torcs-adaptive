/*
	File: PathSegCollection.cpp
	Author: Henri Keeble
	Desc: Defines a class that holds a collection of path segments.
*/

#include "PathSegCollection.h"

namespace procBot
{
	PathSegCollection::PathSegCollection(PathSeg* segs, int nSegs)
	{
		this->nSegs = nSegs;
		this->segs = new PathSeg[nSegs];
		for (int i = 0; i < nSegs; i++)
			this->segs[i] = segs[i];
	}

	PathSegCollection::PathSegCollection(const PathSegCollection& param)
	{
		cpy(param);
	}

	PathSegCollection& PathSegCollection::operator=(const PathSegCollection& param)
	{
		if (this == &param)
			return *this;
		else
		{
			if (segs)
				delete[] segs;
			cpy(param);
		}
	}

	PathSeg* PathSegCollection::operator()(int index)
	{
		return &segs[index];
	}

	PathSegCollection::~PathSegCollection()
	{
		delete[] segs;
	}

	void PathSegCollection::Append(PathSeg* newSegs, int nNewSegs)
	{
		PathSeg* tmp = new PathSeg[nNewSegs + nSegs];

		for (int i = 0; i < nSegs; i++)
			tmp[i] = segs[i];

		for (int i = nSegs; i < nSegs + nNewSegs; i++)
			tmp[i] = newSegs[i];

		segs = tmp;
		nSegs += nNewSegs;
	}

	void PathSegCollection::cpy(const PathSegCollection& param)
	{
		if (param.segs)
		{
			nSegs = param.nSegs;
			segs = new PathSeg[nSegs];
			for (int i = 0; i < nSegs; i++)
				segs[i] = param.segs[i];
		}
		else
			segs = nullptr;
	}
}