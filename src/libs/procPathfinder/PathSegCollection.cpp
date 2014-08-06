/*
	File: PathSegCollection.cpp
	Author: Henri Keeble
	Desc: Defines a class that holds a collection of path segments.
*/

#include "PathSegCollection.h"
#include <iostream>

namespace procPathfinder
{
	PathSegCollection::PathSegCollection(int nSegs)
	{
		// Initialize the vector
		segs = vector<PathSeg>(nSegs);

		// Initialize all data stored by the path segments
		for (int i = 0; i < segs.size(); i++)
			segs[i] = PathSeg();
	}

	PathSegCollection::PathSegCollection(const vector<PathSeg>& segs)
	{
		// Create a copy of the segments passed int
		this->segs = vector<PathSeg>(segs);
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
			cpy(param);

		return *this;
	}

	PathSeg* PathSegCollection::operator()(int index)
	{
		return &segs[index];
	}

	PathSegCollection::~PathSegCollection()
	{
		// nothing yet
	}

	PathSegCollection& PathSegCollection::operator+(const PathSegCollection& param)
	{
		PathSegCollection newCollection = PathSegCollection(*this);
		newCollection.Append(param.segs);
		return newCollection;
	}

	void PathSegCollection::Append(const vector<PathSeg> newSegs)
	{
		for (auto i : newSegs)
			segs.push_back(i);
	}

	void PathSegCollection::Insert(const vector<PathSeg> newSegs, int id)
	{
		for (int i = 0; i < newSegs.size(); i++) {
			if (segs.size() > i + id) {
				segs.at(id + i) = newSegs[i];
				std::cout << "Replacing at " << id + i << std::endl;
			}
			else
				segs.push_back(newSegs[i]);
		}
	}

	void PathSegCollection::cpy(const PathSegCollection& param)
	{
		segs = vector<PathSeg>(param.segs);
	}
}