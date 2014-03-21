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

		// State is uncopyable
		PSSGState(const PSSGState& param) { };
		PSSGState& operator=(const PSSGState& param) { };

		// PSSGState is not responsible for handling deletion of the ssgEntity or ssgLoaderOptions, it simply monitors them
		ssgEntity* desc;
		ssgLoaderOptions* lopts;

	public:
		PSSGState();
		~PSSGState();

		/* Get and set operators */
		ssgEntity* GetDesc() const;
		void UpdateDesc();

		ssgLoaderOptions* GetLoaderOptions() const;
		void SetLoaderOptions(ssgLoaderOptions* loaderOptions);

		void ClearTables();
		void InitTables(int nVerts);

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
		sgVec3				*vtab;
		sgVec3				*ntab;
		sgVec2				*t0tab;
		sgVec2				*t1tab;
		sgVec2				*t2tab;
		sgVec2				*t3tab;
		sgVec2				texrep;
		sgVec2				texoff;
		sgMat4				current_matrix;
		ssgIndexArray		*vertlist;
		ssgIndexArray		*striplist;
		int					current_flags;
		int					num_materials;
		int					num_kids;
		int					mapLevel;
		int					numMapLevel;
		int					last_num_kids;
		int					totalnv;
		int					totalstripe;
		bool				usenormal;
		bool				usestrip;
		bool				firsttime;
		float				t_xmax;
		float				t_ymax;
		float				t_xmin;
		float				t_ymin;

		_ssgMaterial		*mlist[MAX_MATERIALS];
		sgVec4				*clist[MAX_MATERIALS];
	};
}

#endif // _P_LOADER_STATE_