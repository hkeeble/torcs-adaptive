/*
	File: grloadac.h
	Author: Henri Keeble
	Desc: Exported load functions for use in procedural loading, in grLoadProcedural.cpp
*/
#ifndef _GR_LOAD_AC_H_
#define _GR_LOAD_AC_H_

#include "grssgext.h"
#include "grvtxtable.h"
#include "grmultitexstate.h"
#include "grmain.h"
#include "grtexture.h"

struct Tag
{
	const char *token;
	int(*func) (char *s);
};

int do_obj_poly(char *s);
int do_obj_group(char *s);
int do_obj_world(char *s);
int do_obj_light(char *s);

int do_name(char *s);
int do_texture(char *s);

int do_material(char *s);
int do_object(char *s);
int do_data(char *s);
int do_texrep(char *s);
int do_texoff(char *s);
int do_rot(char *s);
int do_loc(char *s);
int do_url(char *s);
int do_numvert(char *s);
int do_numsurf(char *s);
int do_surf(char *s);
int do_mat(char *s);
int do_refs(char *s);
int do_kids(char *s);

int search(Tag *tags, char *s);

/* Tag types */
extern Tag top_tags[];
extern Tag object_tags[];
extern Tag surf_tag[];
extern Tag surface_tags[];
extern Tag obj_type_tags[];

#endif // _GR_LOAD_AC_H_