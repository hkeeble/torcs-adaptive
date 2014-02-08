/*
	File: PSSGState.h
	Author: Henri Keeble
	Desc: Declares a class to represent the current SSG state. Used to append to an
		  ssgEntity as opposed to loading an entirely new one.
*/
#ifndef _P_SSG_STATE_
#define _P_SSG_STATE_

#include "PDefs.h"
#include "../../windows/include/plib/ssg.h"
#include <fstream>
#include <string>

#define MAX_MATERIALS 1000 

namespace procedural
{
	struct _ssgMaterial
	{
		sgVec4 spec;
		sgVec4 emis;
		sgVec4 amb;
		sgVec4 rgb;
		float  shi;
	};

	class PSSGState
	{
	private:
		int currentLine;

		// PSSGState is not responsible for handling deletion of the ssgEntity
		ssgEntity* desc;
		ssgLoaderOptions* lopts;
	public:
		PSSGState();
		PSSGState(const PSSGState& param);
		PSSGState& operator=(const PSSGState& param);
		~PSSGState();

		/* Get and set operators */
		ssgEntity* GetDesc() const;
		void SetDesc(ssgEntity* desc);

		ssgLoaderOptions* GetLoaderOptions() const;
		void SetLoaderOptions(ssgLoaderOptions* loaderOptions);

		/* Initialize the infile, counting number of lines in intial AC file */
		void InitFile(const char* fName);

		static std::fstream infile;

		// Data held for continuity between calls to append
		_ssgMaterial		*current_material;
		sgVec4				*current_colour;
		ssgBranch			*current_branch;
		char				*current_tfname;
		char				*current_tbase;
		char				*current_ttiled;
		char				*current_tskids;
		char				*current_tshad;
		char				*current_data;
		int num_materials;
		sgVec3 *vtab;
		sgVec2 texrep;
		sgVec2 texoff;

		_ssgMaterial   *mlist[MAX_MATERIALS];
		sgVec4         *clist[MAX_MATERIALS];
	};
}

#endif // _P_LOADER_STATE_