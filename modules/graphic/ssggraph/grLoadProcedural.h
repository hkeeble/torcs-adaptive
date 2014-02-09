/*
	File: grLoadProcedural.h
	Author: Henri Keeble
	Desc: Declarations for functions used to load 3D entities procedurally.
*/
#ifndef _GR_LOAD_PROCEDURAL_H_
#define _GR_LOAD_PROCEDURAL_H_

#include "procedural\PSSGState.h"
#include "grmain.h"
#include "grssgext.h"

namespace procedural
{
	#define PARSE_CONT   0
	#define PARSE_POP    1

	/* Structure represents a tag type, contains pointer to function to handle said tag type */
	struct Tag
	{
		const char *token;
		int(*func) (char *s);
	};

	/* Append the entity in the ssgState with the file currently open */
	void PGrAppendEntity(PSSGState* ssgState);

	/* Initialize input file */
	void PGrInitInFile(const char* fname);

	/* Release resources required by the procedural graphics system */
	void PGrShutdown();

	/* Tag types - copied from existing tags to point to procedural handling functions*/
	Tag top_tags[];
	Tag object_tags[];
	Tag surf_tag[];
	Tag surface_tags[];
	Tag obj_type_tags[];

	/* Procedural search function */
	int search(Tag *tags, char *s);

	/* Used to skip white spaces in file (though they shouldn't be present - used for safety */
	void skip_spaces(char **s);

	/* Used to skip qoutes */
	void skip_quotes(char **s);

	/* Functions to handle specific tags - modified from existing ones to make use of the current ssgState */
	
	// These handle top level tokens
	#define OBJ_WORLD  0
	#define OBJ_POLY   1
	#define OBJ_GROUP  2
	#define OBJ_LIGHT  3
	int pr_do_obj_poly		(char *s);
	int pr_do_obj_group		(char *s);
	int pr_do_obj_world		(char *s);
	int pr_do_obj_light		(char *s);

	int pr_do_name			(char *s);
	int pr_do_texture		(char *s);

	int pr_do_material		(char *s);
	int pr_do_object		(char *s);
	int pr_do_data			(char *s);
	int pr_do_texrep		(char *s);
	int pr_do_texoff		(char *s);
	int pr_do_rot			(char *s);
	int pr_do_loc			(char *s);
	int pr_do_url			(char *s);
	int pr_do_numvert		(char *s);
	int pr_do_numsurf		(char *s);
	int pr_do_surf			(char *s);
	int pr_do_mat			(char *s);
	int pr_do_refs			(char *s);
	int pr_do_kids			(char *s);
}

#endif // _GR_LOAD_PROCEDURAL_H_