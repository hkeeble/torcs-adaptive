/**
 *	@file PSSGState.h
 *	@author Henri Keeble
 *	@brief Declares a class to represent the current SSG state. Used to append to an
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

	/*!
	 * A procedural SSG (Simple Scene Graph) state. This class holds the state of a track's scene graph within the SSG API from PLIB. This is such that the SSG can be appended
	 * to as opposed to overwritten, in order to improve performance when appended segments to the track.
	*/
	class PSSGState
	{
	private:
		int currentLine;

		// State is uncopyable
		PSSGState(const PSSGState& param) { };
		PSSGState& operator=(const PSSGState& param) { };

		ssgEntity* desc; 		 /*!< The current track description. */
		ssgLoaderOptions* lopts; /*!< The loader options for the track. */

	public:
		PSSGState();
		~PSSGState();

		/** Get the current description. */
		ssgEntity* GetDesc() const;
		
		/** Update the current description. */
		void UpdateDesc();

		/** Get the current loader options. */
		ssgLoaderOptions* GetLoaderOptions() const;
		
		//! Set the current loader options.
		/*!
			\param loaderOptions The SSG Loader Options to set in the SSG state.
		*/
		void SetLoaderOptions(ssgLoaderOptions* loaderOptions);

		/** Clears all tables in the SSG state. */
		void ClearTables();
		
		/** Initializes all tables in the SSG state */
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