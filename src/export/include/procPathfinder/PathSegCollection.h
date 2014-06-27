/*
	File: PathSegCollection.h
	Author: Henri Keeble
	Desc: Declares a class that holds a collection of path segments.
*/

#ifndef _PATH_SEG_COLLECTION_H_
#define _PATH_SEG_COLLECTION_H_

#include "PathSeg.h"
#include <vector>

using std::vector;

namespace procPathfinder
{
	/* Represents a collection of path segments */
	class PathSegCollection
	{
	public:
		// Constructors
		PathSegCollection() : segs(vector<PathSeg>()) { };
		PathSegCollection(int nSegs);
		PathSegCollection(const vector<PathSeg>& segs);

		// Copy Constructor
		PathSegCollection(const PathSegCollection& param);

		// Operator overloads
		PathSegCollection& operator=(const PathSegCollection& param);
		PathSegCollection& operator+(const PathSegCollection& param);
		PathSeg* operator()(int index);

		// Destructor
		~PathSegCollection();

		/* Appends the new segments into the collection. A copy is made of the segments passed in - calling code is responsible for deletion of the other segments */ 
		void Append(const vector<PathSeg> newSegs);
		
		/* The number of pathsegs stored */
		int Count() const { return segs.size(); }

		const vector<PathSeg>& Segments() { return segs; }
	private:
		/* Internal copying function */
		void cpy(const PathSegCollection& param);
		
		/* The segment vector */
		vector<PathSeg> segs;
	};
}

#endif // _PATH_SEG_COLLECTION_H_