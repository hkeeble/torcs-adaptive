/*
	File: PathSegCollection.h
	Author: Henri Keeble
	Desc: Declares a class that holds a collection of path segments.
*/

#ifndef _PATH_SEG_COLLECTION_H_
#define _PATH_SEG_COLLECTION_H_

#include "PathSeg.h"

namespace procBot
{
	/* Represents a collection of path segments */
	class PathSegCollection
	{
	public:
		PathSegCollection() : segs(nullptr), nSegs(0) { }
		PathSegCollection(int nSegs) { this->nSegs = nSegs; segs = new PathSeg[nSegs]; }
		PathSegCollection(PathSeg* segs, int nSegs);
		PathSegCollection(const PathSegCollection& param);
		PathSegCollection& operator=(const PathSegCollection& param);
		PathSeg* operator()(int index);

		~PathSegCollection();

		/* Appends the new segments into the collection */ 
		void Append(PathSeg* newSegs, int nNewSegs);
		
		int Count() const { return nSegs; }
	private:
		/* Internal copying function */
		void cpy(const PathSegCollection& param);
		
		/* The segment array */
		PathSeg* segs;
		
		/* The number of path segments */
		int nSegs; 
	};
}

#endif // _PATH_SEG_COLLECTION_H_