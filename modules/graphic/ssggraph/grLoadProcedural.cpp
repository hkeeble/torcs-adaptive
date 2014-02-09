/*
	File: grLoadProcedural.cpp
	Author: Henri Keeble
	Desc: Definitions for functions used to load 3D entities procedurally.
*/

#include <fstream>
#include "grLoadProcedural.h"

namespace procedural
{
	/* Used to keep the input file open internally - prevents having to read entire file to find where to begin each time append function is called */
	static std::fstream infile;

	/* State PSSGState - prevents passing state around too much */
	static PSSGState* state;

	// Whether or not the parser is currently in a group. Shouldn't need to be retained between calls to append as all groups should be completed
	bool inGroup = false;

	/*  Tag object definitions */
	Tag top_tags[] =
	{
		{ "MATERIAL",	pr_do_material },
		{ "OBJECT",		pr_do_object },
	};

	Tag object_tags[] =
	{
		{ "name",		pr_do_name },
		{ "data",		pr_do_data },
		{ "texture",	pr_do_texture },
		{ "texrep",		pr_do_texrep },
		{ "texoff",		pr_do_texoff },
		{ "rot",		pr_do_rot },
		{ "loc",		pr_do_loc },
		{ "url",		pr_do_url },
		{ "numvert",	pr_do_numvert },
		{ "numsurf",	pr_do_numsurf },
		{ "kids",		pr_do_kids },
		{ NULL, NULL }
	};

	Tag surf_tag[] =
	{
		{ "SURF",		pr_do_surf },
		{ NULL, NULL }
	};

	Tag surface_tags[] =
	{
		{ "mat",		pr_do_mat },
		{ "refs",		pr_do_refs },
		{ NULL, NULL }
	};

	Tag obj_type_tags[] =
	{
		{ "world",		pr_do_obj_world },
		{ "poly",		pr_do_obj_poly },
		{ "group",		pr_do_obj_group },
		{ "light",		pr_do_obj_light },
		{ NULL, NULL }
	};

	void PGrInitInFile(const char* fname)
	{
		// Count lines in AC File
		infile.open(fname, std::ios::in);

		std::string line;
		if (infile.is_open())
		{
			while (infile.good())
				std::getline(infile, line);
			infile.clear();
		}
		else
			taOut("Error initializing PSSGState file, file not opened!\n");
	}

	void PGrShutdown()
	{
		infile.close();
	}

	// Modified version of grssgLoadAC3D
	void PGrAppendEntity(PSSGState* ssgState)
	{
		taOut("Loading/Appending 3D track model...\n");

		inGroup = false;

		// Obtain pointers from ssgState
		taOut("\tObtaining current options and current 3D Description...\n");
		state = ssgState;
		ssgLoaderOptions* lopts = state->GetLoaderOptions();
		ssgEntity* currentModel = state->GetDesc();

		if (maxTextureUnits == 0)
			InitMultiTex();

		// Set loader options
		ssgSetCurrentOptions((ssgLoaderOptions*)lopts);
		lopts = ssgGetCurrentOptions();

		// Set texture offsets
		sgSetVec2(state->texrep, 1.0, 1.0);
		sgSetVec2(state->texoff, 0.0, 0.0);

		if (infile.is_open() == false)
			taOut("Error, infile for procedural track not open!\n");

		taOut("\tBegin parse...\n");
		std::string line;
		while (infile.good())
		{
			std::getline(infile, line);
			// search(top_tags, (char*)line.c_str());
		}
		taOut("\tEnd Parse...\n");

		infile.clear();

		delete[] state->current_tfname;
		state->current_tfname = NULL;
		delete[] state->vtab;
		state->vtab = 0;

		taOut("Load/Append track model completed successfully.\n");
		state = nullptr; // Nullify state pointer
	}

	/* Tag search function */

	int search(Tag *tags, char *s)
	{
		skip_spaces(&s);

		for (int i = 0; tags[i].token != NULL; i++)
		if (ulStrNEqual(tags[i].token, s, strlen(tags[i].token)))
		{
			s += strlen(tags[i].token);

			skip_spaces(&s);

			return (*(tags[i].func))(s);
		}

		ulSetError(UL_WARNING, "ac_to_gl: Unrecognised token '%900s' (%d)", s, strlen(s));

		return 0;  /* Should never get here */
	}

	/* Helper functions */

	void skip_spaces(char **s)
	{
		while (**s == ' ' || **s == '\t')
			(*s)++;
	}

	void skip_quotes(char **s)
	{
		skip_spaces(s);

		if (**s == '\"')
		{
			(*s)++;

			char *t = *s;

			while (*t != '\0' && *t != '\"')
				t++;

			if (*t != '\"')
				ulSetError(UL_WARNING, "ac_to_gl: Mismatched double-quote ('\"') in '%900s'", *s);

			*t = '\0';
		}
		else
			ulSetError(UL_WARNING, "ac_to_gl: Expected double-quote ('\"') in '%900s'", *s);
	}

	/* Object tag returns */

	int pr_do_obj_world(char *) { return OBJ_WORLD; }
	int pr_do_obj_poly(char *) { return OBJ_POLY; }
	int pr_do_obj_group(char *) { return OBJ_GROUP; }
	int pr_do_obj_light(char *) { return OBJ_LIGHT; }

	/* Tag handling definitions */

	int pr_do_material(char *s)
	{
		taOut("\tInitializing new material...\n");

		char name[1024];
		sgVec4 rgb;
		sgVec4 amb;
		sgVec4 emis;
		sgVec4 spec;
		int   shi;
		float trans;

		if (sscanf(s,
			"%1023s rgb %f %f %f amb %f %f %f emis %f %f %f spec %f %f %f shi %d trans %f",
			name,
			&rgb[0], &rgb[1], &rgb[2],
			&amb[0], &amb[1], &amb[2],
			&emis[0], &emis[1], &emis[2],
			&spec[0], &spec[1], &spec[2],
			&shi,
			&trans) != 15)
		{
			// Buffer size in plib is 1024, I allow here just 900 characters, such that the WHOLE
			// string fits the buffer.
			ulSetError(UL_WARNING, "grloadac:do_material: Can't parse this MATERIAL:%900s", s);
		}
		else
		{
			char *nm = name;

			skip_quotes(&nm);

			amb[3] = emis[3] = spec[3] = 1.0f;
			rgb[3] = 1.0f - trans;

			state->mlist[state->num_materials] = new _ssgMaterial;
			state->clist[state->num_materials] = new sgVec4[1];

			sgCopyVec4(state->clist[state->num_materials][0], rgb);

			state->current_material = state->mlist[state->num_materials];
			sgCopyVec4(state->current_material->spec, spec);
			sgCopyVec4(state->current_material->emis, emis);
			sgCopyVec4(state->current_material->rgb, rgb);
			sgCopyVec4(state->current_material->amb, amb);
			state->current_material->shi = (float)shi;
		}

		state->num_materials++;
		return PARSE_CONT;
	}

	int pr_do_object(char * s)
	{
		//ssgBranch *current_branch_g = NULL;
		int obj_type = search(obj_type_tags, s);

		delete[] state->current_tfname;
		state->current_tfname = nullptr;

		sgSetVec2(state->texrep, 1.0f, 1.0f);
		sgSetVec2(state->texoff, 0.0f, 0.0f);

		sgMakeIdentMat4(state->current_matrix);

		ssgEntity *old_cb = state->current_branch;

		if (obj_type == OBJ_GROUP) {
			ssgBranch *current_branch_g = NULL;
			inGroup = 1;
			current_branch_g = new ssgBranchCb();
			state->current_branch->addKid(current_branch_g);
			state->current_branch = (ssgTransform*)current_branch_g;

			extern int preScene(ssgEntity *e);
			current_branch_g->setCallback(SSG_CALLBACK_PREDRAW, preScene);
		}
		else {
			inGroup = 0;
		}

		ssgTransform *tr = new ssgTransform();

		tr->setTransform(state->current_matrix);

		state->current_branch->addKid(tr);
		state->current_branch = tr;

		std::string line;
		while (std::getline(infile, line))
		if (search(object_tags, (char*)line.c_str()) == PARSE_POP)
			break;

		int num_kids = state->last_num_kids;

		for (int i = 0; i < num_kids; i++) {
			/* EE: bad hack for buggy .acc format... */
			if (std::getline(infile, line))
				search(top_tags, (char*)line.c_str());
			else
				break;
		}

		state->current_branch = (ssgBranch *)old_cb;
		return PARSE_CONT;
	}

	int pr_do_name(char *s)
	{
		char *q = nullptr;
		skip_quotes(&s);

		if (!strncmp(s, "TKMN", 4))
		{
			q = strstr(s, "_g");
			if (q != NULL)
				*q = '\0';
		}

		// Set the name of the current branch
		state->current_branch->setName(s);

		return PARSE_CONT;
	}

	int pr_do_data(char *s)
	{
		int len = strtol(s, NULL, 0);

		state->current_data = new char[len + 1];

		for (int i = 0; i < len; i++)
			state->current_data[i] = infile.get();

		state->current_data[len] = '\0';

		infile.get();  /* Final RETURN */

		ssgBranch *br = state->GetLoaderOptions()->createBranch(state->current_data);

		if (br != NULL)
		{
			state->current_branch->addKid(br);
			state->current_branch = br;
		}

		/* delete [] current_data ; */
		state->current_data = nullptr;

		return PARSE_CONT;
	}

	int pr_do_texture(char *s)
	{
		char *p;

		if (s == NULL || s[0] == '\0')
			state->current_tfname = NULL;
		else
		{
			if ((p = strstr(s, " base")) != NULL)
			{
				*p = '\0';
				numMapLevel = 1;
				mapLevel = LEVEL0;
				delete[] state->current_tbase;
				delete[] state->current_tfname;
				delete[] state->current_ttiled;
				state->current_ttiled = 0;
				delete[] state->current_tskids;
				state->current_tskids = 0;
				delete[] state->current_tshad;
				state->current_tshad = 0;
				skip_quotes(&s);
				state->current_tbase = new char[strlen(s) + 1];
				state->current_tfname = new char[strlen(s) + 1];
				strcpy(state->current_tbase, s);
				strcpy(state->current_tfname, s);
			}
			else  if ((p = strstr(s, " tiled")) != NULL)
			{
				*p = '\0';
				delete[] state->current_ttiled;
				state->current_ttiled = 0;
				delete[] state->current_tskids;
				state->current_tskids = 0;
				delete[] state->current_tshad;
				state->current_tshad = 0;
				if (!strstr(s, NOTEXTURE))
				{
					numMapLevel++;;
					mapLevel |= LEVEL1;
					skip_quotes(&s);
					state->current_ttiled = new char[strlen(s) + 1];
					strcpy(current_ttiled, s);
				}
			}
			else  if ((p = strstr(s, " skids")) != NULL)
			{
				*p = '\0';
				delete[] state->current_tskids;
				state->current_tskids = 0;
				delete[] state->current_tshad;
				state->current_tshad = 0;
				if (!strstr(s, NOTEXTURE))
				{
					numMapLevel++;;
					mapLevel |= LEVEL2;
					skip_quotes(&s);
					current_tskids = new char[strlen(s) + 1];
					strcpy(current_tskids, s);
				}
			}
			else  if ((p = strstr(s, " shad")) != NULL)
			{
				*p = '\0';
				delete[] state->current_tshad;
				state->current_tshad = 0;
				if (!strstr(s, NOTEXTURE))
				{
					numMapLevel++;;
					mapLevel |= LEVEL3;
					skip_quotes(&s);
					current_tshad = new char[strlen(s) + 1];
					strcpy(current_tshad, s);
				}
			}
			else
			{
				skip_quotes(&s);
				numMapLevel = 1;
				mapLevel = LEVEL0;
				delete[] state->current_tfname;
				delete[] state->current_tbase;
				state->current_tbase = 0;
				delete[] state->current_ttiled;
				state->current_ttiled = 0;
				delete[] state->current_tskids;
				state->current_tskids = 0;
				delete[] state->current_tshad;
				state->current_tshad = 0;
				state->current_tfname = new char[strlen(s) + 1];
				strcpy(state->current_tfname, s);
			}
		}

		return PARSE_CONT;
	}

	int pr_do_texrep(char *s)
	{
		if (sscanf(s, "%f %f", &state->texrep[0], &state->texrep[1]) != 2)
			ulSetError(UL_WARNING, "ac_to_gl: Illegal texrep record.");

		return PARSE_CONT;
	}

	int pr_do_texoff(char *s)
	{
		if (sscanf(s, "%f %f", &state->texoff[0], &state->texoff[1]) != 2)
			ulSetError(UL_WARNING, "ac_to_gl: Illegal texoff record.");

		return PARSE_CONT;
	}

	int pr_do_rot(char *s)
	{
		state->current_matrix[0][3] = state->current_matrix[1][3] = state->current_matrix[2][3] =
			state->current_matrix[3][0] = state->current_matrix[3][1] = state->current_matrix[3][2] = 0.0f;
		state->current_matrix[3][3] = 1.0f;

		if (sscanf(s, "%f %f %f %f %f %f %f %f %f",
			&state->current_matrix[0][0], &state->current_matrix[0][1], &state->current_matrix[0][2],
			&state->current_matrix[1][0], &state->current_matrix[1][1], &state->current_matrix[1][2],
			&state->current_matrix[2][0], &state->current_matrix[2][1], &state->current_matrix[2][2]) != 9)
			ulSetError(UL_WARNING, "ac_to_gl: Illegal rot record.");

		((ssgTransform *)state->current_branch)->setTransform(state->current_matrix);
		return PARSE_CONT;
	}

	int pr_do_loc(char *s)
	{
		if (sscanf(s, "%f %f %f", &state->current_matrix[3][0], &state->current_matrix[3][2], &state->current_matrix[3][1]) != 3)
			ulSetError(UL_WARNING, "ac_to_gl: Illegal loc record.");

		state->current_matrix[3][1] = -state->current_matrix[3][1];
		state->current_matrix[3][3] = 1.0f;
		((ssgTransform *)state->current_branch)->setTransform(state->current_matrix);

		return PARSE_CONT;
	}

	int pr_do_url(char *s)
	{
		skip_quotes(&s);

#ifdef PRINT_URLS
		printf("/* URL: \"%s\" */\n", s);
#endif

		return PARSE_CONT;
	}

	int pr_do_numvert(char *s)
	{
		char buffer[1024];

		nv = strtol(s, NULL, 0);

		delete[] vtab;
		delete[] ntab;
		delete[] t0tab;
		delete[] t1tab;
		delete[] t2tab;
		delete[] t3tab;
		totalnv = nv;
		totalstripe = 0;

		vtab = new sgVec3[nv];
		ntab = new sgVec3[nv];
		t0tab = new sgVec2[nv];
		t1tab = new sgVec2[nv];
		t2tab = new sgVec2[nv];
		t3tab = new sgVec2[nv];

		if (vertlist != NULL) {
			ssgDeRefDelete(vertlist);
		}
		if (striplist != NULL) {
			ssgDeRefDelete(striplist);
		}

		vertlist = new ssgIndexArray();
		vertlist->ref();
		striplist = new ssgIndexArray();
		striplist->ref();

		for (int i = 0; i < nv; i++)
		{
			FGETS(buffer, 1024, loader_fd);

			if (sscanf(buffer, "%f %f %f %f %f %f",
				&vtab[i][0], &vtab[i][1], &vtab[i][2], &ntab[i][0], &ntab[i][1], &ntab[i][2]) != 6)
			{
				usenormal = 0;
				if (sscanf(buffer, "%f %f %f",
					&vtab[i][0], &vtab[i][1], &vtab[i][2]) != 3)
				{
					ulSetError(UL_FATAL, "ac_to_gl: Illegal vertex record.");
				}
			}
			else
			{
				usenormal = 1;
				float tmp = ntab[i][1];
				ntab[i][1] = -ntab[i][2];
				ntab[i][2] = tmp;
			}

			float tmp = vtab[i][1];
			vtab[i][1] = -vtab[i][2];
			vtab[i][2] = tmp;

			if (vtab[i][0] >t_xmax)
				t_xmax = vtab[i][0];
			if (vtab[i][0] <t_xmin)
				t_xmin = vtab[i][0];

			if (vtab[i][1] >t_ymax)
				t_ymax = vtab[i][1];
			if (vtab[i][1] <t_ymin)
				t_ymin = vtab[i][1];

		}

		return PARSE_CONT;
	}

	int pr_do_numsurf(char *s)
	{
		int ns = strtol(s, NULL, 0);

		for (int i = 0; i < ns; i++)
		{
			char buffer[1024];

			FGETS(buffer, 1024, loader_fd);
			search(surf_tag, buffer);
		}

		return PARSE_CONT;
	}

	int pr_do_surf(char *s)
	{
		state->current_flags = strtol(s, NULL, 0);

		char buffer[1024];

		while (FGETS(buffer, 1024, loader_fd) != NULL)
		if (search(surface_tags, buffer) == PARSE_POP)
			break;

		return PARSE_CONT;
	}

	int pr_do_mat(char *s)
	{
		int mat = strtol(s, NULL, 0);

		state->current_material = state->mlist[mat];
		state->current_colour = state->clist[mat];

		return PARSE_CONT;
	}

	int pr_do_refs(char *s)
	{
		int nrefs = strtol(s, NULL, 0);
		char buffer[1024];

		if (nrefs == 0) {
			return PARSE_POP;
		}

		ssgVertexArray *vlist = new ssgVertexArray(nrefs);
		ssgTexCoordArray *tlist = new ssgTexCoordArray(nrefs);
		ssgTexCoordArray *tlist1 = NULL;
		ssgTexCoordArray *tlist2 = NULL;
		ssgTexCoordArray *tlist3 = NULL;
		//ssgIndexArray *vindices = new ssgIndexArray(nrefs);
		ssgNormalArray *nrm = new ssgNormalArray(nrefs);

		if (numMapLevel > 1) {
			tlist1 = new ssgTexCoordArray(nrefs);
		}
		if (numMapLevel > 2) {
			tlist2 = new ssgTexCoordArray(nrefs);
		}
		if (numMapLevel > 3) {
			tlist3 = new ssgTexCoordArray(nrefs);
		}

		for (int i = 0; i < nrefs; i++) {
			FGETS(buffer, 1024, loader_fd);

			int vtx;
			sgVec2 tc;
			sgVec2 tc1 = { 0 };
			sgVec2 tc2 = { 0 };
			sgVec2 tc3 = { 0 };
			int tn = 0;
			tn = sscanf(buffer, "%d %f %f %f %f %f %f %f %f", &vtx,
				&tc[0], &tc[1],
				&tc1[0], &tc1[1],
				&tc2[0], &tc2[1],
				&tc3[0], &tc3[1]);

			if (tn < 3)
			{
				ulSetError(UL_FATAL, "ac_to_gl: Illegal ref record not enough text coord.");
			}

			tc[0] *= texrep[0];
			tc[1] *= texrep[1];
			tc[0] += texoff[0];
			tc[1] += texoff[1];

			tlist->add(tc);
			t0tab[vtx][0] = tc[0];
			t0tab[vtx][1] = tc[1];

			t1tab[vtx][0] = tc1[0];
			t1tab[vtx][1] = tc1[1];

			t2tab[vtx][0] = tc2[0];
			t2tab[vtx][1] = tc2[1];

			t3tab[vtx][0] = tc3[0];
			t3tab[vtx][1] = tc3[1];

			if (numMapLevel > 1) {
				tlist1->add(tc1);
			}
			if (numMapLevel > 2) {
				tlist2->add(tc2);
			}
			if (numMapLevel > 3) {
				tlist3->add(tc3);
			}

			vlist->add(vtab[vtx]);
			if (usenormal == 1) {
				nrm->add(ntab[vtx]);
			}
			//vindices-> add (i);
			vertlist->add(vtx);
		}
#ifdef GUIONS
		if (usenormal == 1) {
			printf("use normal\n");
		}
#endif /* GUIONS */

		ssgColourArray *col = new ssgColourArray(1);

		col->add(*state->current_colour);

		sgVec3 nm;

		if (usenormal == 0) {
			if (nrefs < 3) {
				sgSetVec3(nm, 0.0f, 0.0f, 1.0f);
			}
			else {
				sgMakeNormal(nm, vlist->get(0), vlist->get(1), vlist->get(2));
			}
			nrm->add(nm);
		}


		int type = (current_flags & 0x0F);
		if (type >= 0 && type <= 4) {
			GLenum gltype = GL_TRIANGLES;
			switch (type)
			{
			case 0: gltype = GL_TRIANGLE_FAN;
				break;
			case 1: gltype = GL_LINE_LOOP;
				break;
			case 2: gltype = GL_LINE_STRIP;
				break;
			case 4: gltype = GL_TRIANGLE_STRIP;
				usestrip = TRUE;
				break;
			}

#ifdef NORMAL_TEST
			/* GUIONS TEST that draw all the normals of a car */
			if (isacar == TRUE) {
				ssgVertexArray *vlinelist = new ssgVertexArray(nv * 2);
				for (i = 0; i < nv; i++) {
					sgVec3 tv;
					tv[0] = ntab[i][0] * 0.2 + vtab[i][0];
					tv[1] = ntab[i][1] * 0.2 + vtab[i][1];
					tv[2] = ntab[i][2] * 0.2 + vtab[i][2];
					vlinelist->add(vtab[i]);
					vlinelist->add(tv);
				}
				ssgVtxTable *vline = new ssgVtxTable(GL_LINES, vlinelist, NULL, NULL, NULL);
				current_branch->addKid(current_options->createLeaf(vline, 0));
			}
#endif

			/* check the number of texture units */
			if (numMapLevel > maxTextureUnits) {
				numMapLevel = maxTextureUnits;
			}
			if (isacar == TRUE) {
				mapLevel = LEVELC;
				if (tlist1 && maxTextureUnits > 1) {
					mapLevel = LEVELC2;
					numMapLevel = 2;
				}
				if (tlist2 && maxTextureUnits > 2) {
					mapLevel = LEVELC3;
					numMapLevel = 3;
				}
			}
#define VTXARRAY_GUIONS
#ifdef VTXARRAY_GUIONS
			if (usestrip == FALSE)
#endif
			{
				/* TEST
				if (isacar==FALSE)
				{numMapLevel=1;
				mapLevel=LEVEL0;
				}
				*/
				grVtxTable* vtab = new grVtxTable(gltype,
					vlist, nrm, tlist, tlist1, tlist2, tlist3, numMapLevel, mapLevel, col, indexCar);
				/* good */
				/*ssgVtxArray* vtab = new ssgVtxArray ( gltype,
				vlist, nrm, tlist, col , vindices) ;*/

				/*striplist-> add (nrefs);
				grVtxTable* vtab = new grVtxTable ( gltype,
				vlist,
				striplist,
				1,vertlist,
				nrm, tlist,tlist1,tlist2,tlist3,numMapLevel,mapLevel, col, indexCar ) ;*/

				/*printf("name ob =%s , numMapLevel =%d , maoLevel=%d \n",	current_branch -> getName (  ) ,numMapLevel, mapLevel);*/

				vtab->setState(get_state(state->current_material));
				vtab->setCullFace(!((current_flags >> 4) & 0x02));

				if (numMapLevel>1)
					vtab->setState1(get_state_ext(state->current_ttiled));
				if (numMapLevel>2)
					vtab->setState2(get_state_ext(state->current_tskids));
				if (numMapLevel>3)
					vtab->setState3(get_state_ext(state->current_tshad));

				ssgLeaf* leaf = state->GetLoaderOptions()->createLeaf(vtab, 0);

				if (leaf) {
					state->current_branch->addKid(leaf);
				}
				else {
					// TODO: delete leaf or vtab?
					delete vtab;
				}
			}
#ifdef VTXARRAY_GUIONS
			else {
				/* memorize the stripe index */
				striplist->add(nrefs);
				totalstripe++;
				delete vlist;
				vlist = 0;
				delete tlist;
				tlist = 0;
				delete tlist1;
				tlist1 = 0;
				delete tlist2;
				tlist2 = 0;
				delete tlist3;
				tlist3 = 0;
				/*delete vindices;
				vindices = 0;*/
				delete nrm;
				nrm = 0;
			}
#endif
		}

		if (col->getRef() == 0) {
			delete col;
		}
		return PARSE_POP;
	}

	int pr_do_kids(char *s)
	{
		state->last_num_kids = strtol(s, NULL, 0);

#ifdef VTXARRAY_GUIONS
		if (state->last_num_kids == 0 && usestrip == TRUE && inGroup != 1) {
			ssgVertexArray *vlist = new ssgVertexArray(totalnv);
			ssgNormalArray *nrm = new ssgNormalArray(totalnv);
			ssgTexCoordArray *tlist0 = new ssgTexCoordArray(totalnv);
			ssgTexCoordArray *tlist1 = NULL;
			ssgTexCoordArray *tlist2 = NULL;
			ssgTexCoordArray *tlist3 = NULL;
			/* if (numMapLevel>1) */
			tlist1 = new ssgTexCoordArray(totalnv);
			/* if (numMapLevel>2) */
			tlist2 = new ssgTexCoordArray(totalnv);
			/* if (numMapLevel>3) */
			tlist3 = new ssgTexCoordArray(totalnv);
			for (int i = 0; i < totalnv; i++) {
				tlist0->add(t0tab[i]);
				/* if (numMapLevel>1) */
				tlist1->add(t1tab[i]);
				/* if (numMapLevel>2) */
				tlist2->add(t2tab[i]);
				/* if (numMapLevel>3) */
				tlist3->add(t3tab[i]);
				vlist->add(vtab[i]);
				if (usenormal == 1) {
					nrm->add(ntab[i]);
				}
			}

			ssgColourArray *col = new ssgColourArray(1);
			col->add(*state->current_colour);

			/* int type = ( current_flags & 0x0F ) ; */
			GLenum gltype = GL_TRIANGLE_STRIP;

			/* check the number of texture units */
			if (numMapLevel>maxTextureUnits)
				numMapLevel = maxTextureUnits;
			if (isacar == TRUE) {
				mapLevel = LEVELC;
				if (tlist1 && maxTextureUnits>2) {
					mapLevel = LEVELC2;
					numMapLevel = 2;
				}
				if (tlist2 && maxTextureUnits>2){
					mapLevel = LEVELC3;
					numMapLevel = 3;
				}
			}
			/*ssgVtxArray* vtab = new ssgVtxArray ( gltype,
			vlist, nrm, tlist0, col , vertlist) ;*/

			grVtxTable* vtab = new grVtxTable(gltype,
				vlist,
				striplist,
				totalstripe,
				vertlist,
				nrm,
				tlist0, tlist1, tlist2, tlist3,
				numMapLevel, mapLevel,
				col,
				indexCar);
			vtab->setState(get_state(state->current_material));
			vtab->setCullFace(!((state->current_flags >> 4) & 0x02));
			if (numMapLevel>1)
				vtab->setState1(get_state_ext(state->current_ttiled));
			if (numMapLevel>2)
				vtab->setState2(get_state_ext(state->current_tskids));
			if (numMapLevel>3)
				vtab->setState3(get_state_ext(state->current_tshad));
			ssgLeaf* leaf = state->GetLoaderOptions()->createLeaf(vtab, 0);

			if (leaf) {
				state->current_branch->addKid(leaf);
			}
			else {
				delete vtab;
			}
		}
#endif

		numMapLevel = 1;
		mapLevel = LEVEL0;
		return PARSE_POP;
	}
}