#include "hx_exc.h"
#include "hx_path.h"
#include "hx_file.h"
#include "hx_str.h"
#include "hx_seq.h"
#include "hx_glshader.h"
#include "hx_shaderregistry.h"


hx_shader_registry_t *
HxShaderRegistry_new()
{
	hx_shader_registry_t *self = (hx_shader_registry_t *)Hx_Utils_allocate(sizeof(hx_shader_registry_t));
	self->items = HxSeq_new(NULL);

	self->tokens = (struct HxParserTokens *)Hx_Utils_allocate(sizeof(struct HxParserTokens));

	/* create tokens */
	self->tokens->blank = HxStr_new(" ");
	self->tokens->empty = HxStr_new("");

	self->tokens->void_ = HxStr_new("void");
	self->tokens->main = HxStr_new("main");
	self->tokens->layout = HxStr_new("layout");
	self->tokens->uniform = HxStr_new("uniform");
	self->tokens->bracket_open = HxStr_new("(");
	self->tokens->bracket_close = HxStr_new(")");
	self->tokens->eq = HxStr_new("=");
	self->tokens->comma = HxStr_new(",");
	self->tokens->semicolon = HxStr_new(";");
	self->tokens->version = HxStr_new("#version");
	self->tokens->in = HxStr_new("in");
	self->tokens->out = HxStr_new("out");

	self->tokens->vec2 = HxStr_new("vec2");
	self->tokens->vec3 = HxStr_new("vec3");
	self->tokens->vec4 = HxStr_new("vec4");
	self->tokens->mat3 = HxStr_new("mat3");
	self->tokens->mat4 = HxStr_new("mat4");
	self->tokens->sampler1d = HxStr_new("sampler1d");
	self->tokens->sampler2d = HxStr_new("sampler2d");
	self->tokens->sampler3d = HxStr_new("sampler3d");

	self->type_tokens = HxSeq_new(self->tokens->vec2,
									self->tokens->vec3,
									self->tokens->vec4,
									self->tokens->mat3,
									self->tokens->mat4,
									self->tokens->sampler1d,
									self->tokens->sampler2d,
									self->tokens->sampler3d,
									NULL);

	return self;
}


void
HxShaderRegistry_del(hx_shader_registry_t *self)
{
	hx_seq_iter_t *it;

	it = HxSeqIter_new(self->items);
	while(HxSeqIter_next(it))
	{
		HxGLProgram_del((hx_glprogram_t *)HxSeqIter_get(it));
	}
	HxSeqIter_del(it);
	HxSeq_del(self->items);

	it = HxSeqIter_new(self->type_tokens);
	while(HxSeqIter_next(it))
	{
		HxGLProgram_del((hx_glprogram_t *)HxSeqIter_get(it));
	}
	HxSeqIter_del(it);
	HxSeq_del(self->type_tokens);

	HxStr_del(self->tokens->blank);
	HxStr_del(self->tokens->empty);
	HxStr_del(self->tokens->main);
	HxStr_del(self->tokens->void_);
	HxStr_del(self->tokens->layout);
	HxStr_del(self->tokens->uniform);
	HxStr_del(self->tokens->bracket_open);
	HxStr_del(self->tokens->bracket_close);
	HxStr_del(self->tokens->eq);
	HxStr_del(self->tokens->comma);
	HxStr_del(self->tokens->semicolon);
	HxStr_del(self->tokens->version);
	HxStr_del(self->tokens->in);
	HxStr_del(self->tokens->out);

	free(self->tokens);

	free(self);
}


hx_glprogram_t *
HxShaderRegistry_register(hx_shader_registry_t *self, hx_glprogram_t *program)
{
	HxSeq_append(self->items, (void *) program);
	return program;
}


/*
 * Remove shader program from render pipeline and garbage collect it.
 * N.B. Look before you leap. Invalid *name* terminates program.
 */

void
HxShaderRegistry_unregister(hx_shader_registry_t *self, hx_str_t *program_name)
{
	hx_seq_iter_t *it = HxSeqIter_new(self->items);
	hx_glprogram_t *current;
	long i = -1;
	bool found = false;
	while(HxSeqIter_next(it))
	{
		current = (hx_glprogram_t *)HxSeqIter_get(it);
		if (HxStr_equals(current->name, program_name))
		{
			found = true;
			break;
		}
		i++;
	}

	if (!found)
	{
		printf("%s: '%s'\n", HxExc_get_error_name(HxKeyError), program_name->s);
		exit(HxKeyError);
	}

	HxSeqIter_del(it);
	HxGLProgram_del((hx_glprogram_t *)HxSeq_pop(self->items, i));
}


/*
 * Load a shader program from the specified *folder*.
 * The folder is split up on the path separator and the last bit
 * will be the name of the shader program in the registry.
 *
 * It is expected, that there will be at least one shader
 * source file. Otherwise an error is thrown. The files have to be
 * named in the following manner in order to be discovered and used
 * in the compiling of the shader program:
 *
 * [.*\.](type).glsl
 * where type = vertex|geometry|fragment
 *
 * The files are sorted alphabetically before loading. The type of the
 * shader is taken from the name of the file. The glsl source code from
 * the file will be parsed to discover any variable declarations.
 * Those variables will be activated to be fully usable in OpenGL.
 * They can be accessed through HxShader getter methods.
 *
 * @param folder The file system path to the shader source files.
 */
void
HxShaderRegistry_load(hx_shader_registry_t *self, hx_str_t *folder)
{
	hx_glprogram_t *program = HxGLProgram_new();
	hx_seq_t *filenames = HxFile_get_filenames_in_folder(folder);
	hx_seq_t *filenames_sorted = HxSeq_sorted(filenames);
	hx_str_t * glsl_ext = HxStr_new(".glsl");

	hx_seq_iter_t *filenames_sorted_iter = HxSeqIter_new(filenames_sorted);
	while(HxSeqIter_next(filenames_sorted_iter))
	{
		if (HxStr_endswith((hx_str_t *)HxSeqIter_get(filenames_sorted_iter), glsl_ext))
		{
			HxShaderRegistry_parse(self, program, (hx_str_t *)HxSeqIter_get(filenames_sorted_iter));
		}
	}
	HxSeqIter_del(filenames_sorted_iter);

	hx_seq_t *splitfolder = Hx_Path_split(folder);
	hx_str_t *program_name = HxStr_new(((hx_str_t *)HxSeq_get_item(splitfolder, HxSeq_len(splitfolder) - 1))->s);
	program->name = program_name;

	HxShaderRegistry_register(self, program);

	hx_seq_iter_t *splitfolder_iter = HxSeqIter_new(splitfolder);
	while(HxSeqIter_next(splitfolder_iter))
	{
		HxStr_del((hx_str_t *)HxSeqIter_get(splitfolder_iter));
	}
	HxSeqIter_del(splitfolder_iter);
	HxSeq_del(splitfolder);

	HxSeq_del(filenames);
	HxSeq_del(filenames_sorted);
	HxStr_del(glsl_ext);
}


/*
 * Look before you leap. Invalid names terminate program.
 */
hx_glprogram_t *
HxShaderRegistry_get(hx_shader_registry_t *self, hx_str_t *program_name)
{
	hx_glprogram_t *program = NULL;
	hx_seq_iter_t *it = HxSeqIter_new(self->items);
	while(HxSeqIter_next(it))
	{
		program = (hx_glprogram_t *)HxSeqIter_get(it);
		if (HxStr_equals(program->name, program_name) == 0)
		{
			break;
		}
	}
	if (program == NULL)
	{
		printf("%s: %s\n", HxExc_get_error_name(HxKeyError), program_name->s);
		exit(HxKeyError);
	}

	HxSeqIter_del(it);

	return program;
}


/*
 * Look before you leap. Invalid indexes terminate program.
 */
hx_glprogram_t *
HxShaderRegistry_get_by_index(hx_shader_registry_t *self, int index)
{
	return (hx_glprogram_t *)HxSeq_get_item(self->items, index);
}


hx_seq_t *
HxShaderRegistry_get_keys(hx_shader_registry_t *self)
{
	hx_seq_t *names = HxSeq_new(NULL);
	hx_seq_iter_t *items = HxSeqIter_new(self->items);
	while(HxSeqIter_next(items))
	{
		HxSeq_append(names, (hx_str_t *)(((hx_glprogram_t *)HxSeqIter_get(items))->name));
	}
	HxSeqIter_del(items);
	return names;
}


hx_seq_iter_t *
HxShaderRegistry_get_values(hx_shader_registry_t *self)
{
	return HxSeqIter_new(self->items);
}


void
HxShaderRegistry_parse(hx_shader_registry_t *self, hx_glprogram_t *program, hx_str_t *filepath)
{

	hx_seq_t *source = HxSeq_new(NULL);
	hx_seq_t *vars = HxSeq_new(NULL);

	/****************************************
	 ** infer type of shader from filepath **
	 ****************************************/
	int type = -1;
	hx_str_t *typename;

	typename = HxStr_new("vertex.glsl");
	if (HxStr_contains(filepath, typename))
	{
		type = GL_VERTEX_SHADER;
	}
	HxStr_del(typename);

	if (type == -1)
	{
		typename = HxStr_new("geometry.glsl");
		if (HxStr_contains(filepath, typename))
		{
			type = GL_GEOMETRY_SHADER;
		}
		HxStr_del(typename);
	}

	if (type == -1)
	{
		typename = HxStr_new("fragment.glsl");
		if (HxStr_contains(filepath, typename))
		{
			type = GL_FRAGMENT_SHADER;
		}
		HxStr_del(typename);
	}

	/* type name wasn't provided through filename, raise ValueError */
	if (type == -1)
	{
		printf("%s: No shader type name in file name.", HxExc_get_error_name(HxValueError));
		exit(HxValueError);
	}

	/****************************
	 ** parse glsl source code **
	 ****************************/

	hx_str_t *version_nr = self->tokens->empty;

	/* iterate over lines in glsl source code file */
	FILE *file = fopen(filepath->s, "rb");
	hx_file_iter_t *lines = HxFileIter_new(file);
	bool keep_going = true;
	while(HxFileIter_next(lines) && keep_going)
	{
		hx_str_t *untrimmed_line = HxFileIter_get(lines);
		HxSeq_append(source, untrimmed_line);

		hx_str_t *line = HxStr_trim(untrimmed_line);

		hx_seq_t *unfiltered_bits = HxStr_split(line, self->tokens->blank);
		hx_seq_t *bits = HxSeq_filter(unfiltered_bits, HxStr_to_bool);

		if (HxSeq_len(bits) < 1)
		{
			goto loop_cleanup;
		}

		hx_str_t *first_bit = HxSeq_get_item(bits, 0);

		/* lines beginning with 'out' are irrelevant */
		if (HxStr_startswith(first_bit, self->tokens->out))
		{
			goto loop_cleanup;
		}

		/* if main function is reached, we are done parsing */
		if (HxStr_startswith(first_bit, self->tokens->void_)
				|| HxStr_startswith(first_bit, self->tokens->main))
		{
			keep_going = false;
			goto loop_cleanup;
		}

		/* look for version string */
		if (HxStr_startswith(first_bit, self->tokens->version))
		{
			if (HxSeq_len(bits) < 2)
			{
				printf("%s: Found token '%s' but no token thereafter."
						, HxExc_get_error_name(HxValueError), self->tokens->version->s);
				exit(HxValueError);
			}
			version_nr = HxStr_new(((hx_str_t *)HxSeq_get_item(bits, 1))->s);
			goto loop_cleanup;
		}

		/* layouted vars */
		if (HxStr_startswith(first_bit, self->tokens->layout))
		{
			if (HxSeq_len(bits) != 4)
			{
				printf("%s: Expected 4 tokens in line beginning with 'layout', got %li",
						HxExc_get_error_name(HxValueError), HxSeq_len(bits));
				exit(HxValueError);
			}

			if (!HxStr_equals((hx_str_t *)HxSeq_get_item(bits, 1), self->tokens->in))
			{
				printf("%s: Expected token 'in' after 'layout' token, got %li",
						HxExc_get_error_name(HxValueError), HxSeq_len(bits));
				exit(HxValueError);
			}

			/* get position of var in layout */
			hx_str_t *untrimmed_n = HxStr_slice(
						first_bit,
						HxStr_index(first_bit, self->tokens->eq, 0) + 1,
						HxStr_index(first_bit, self->tokens->bracket_close, 0),
						1);
			hx_str_t *n = HxStr_trim(untrimmed_n);
			long varpos = HxStr_to_long(n, 10);

			/* get type name */
			hx_str_t *vartypename = HxStr_new(((hx_str_t *)HxSeq_get_item(bits, 2))->s);
			int vartype = HxShaderRegistry_get_var_type_by_name(self, vartypename);

			/* get var name */
			hx_str_t *varname = HxStr_replace(HxSeq_get_item(bits, 3),
												self->tokens->semicolon,
												self->tokens->empty);

			hx_glvar_t *var = HxGLVar_new(varname, vartype, varpos, true, false);
			HxSeq_append(vars, var);

			HxStr_del(untrimmed_n);
			HxStr_del(n);
		}

		if (HxStr_equals(first_bit, self->tokens->in))
		{
			/* get type name */
			hx_str_t *vartypename = HxStr_new(((hx_str_t *)HxSeq_get_item(bits, 1))->s);
			int vartype = HxShaderRegistry_get_var_type_by_name(self, vartypename);

			/* get var name */
			hx_str_t *varname = HxStr_replace(HxSeq_get_item(bits, 2),
												self->tokens->semicolon,
												self->tokens->empty);
			hx_glvar_t *var = HxGLVar_new(varname, vartype, -1, true, false);
			HxSeq_append(vars, var);
		}
		else if (HxStr_equals(first_bit, self->tokens->out))
		{
			/* get type name */
			hx_str_t *vartypename = HxStr_new(((hx_str_t *)HxSeq_get_item(bits, 1))->s);
			int vartype = HxShaderRegistry_get_var_type_by_name(self, vartypename);

			/* get var name */
			hx_str_t *varname = HxStr_replace(HxSeq_get_item(bits, 2),
												self->tokens->semicolon,
												self->tokens->empty);
			hx_glvar_t *var = HxGLVar_new(varname, vartype, -1, true, false);
			HxSeq_append(vars, var);
		}
		else if (HxStr_equals(first_bit, self->tokens->uniform))
		{
			/* get type name */
			hx_str_t *vartypename = HxStr_new(((hx_str_t *)HxSeq_get_item(bits, 1))->s);
			int vartype = HxShaderRegistry_get_var_type_by_name(self, vartypename);

			/* get var name */
			hx_str_t *varname = HxStr_replace(HxSeq_get_item(bits, 2),
												self->tokens->semicolon,
												self->tokens->empty);
			hx_glvar_t *var = HxGLVar_new(varname, vartype, -1, false, true);
			HxSeq_append(vars, var);
		}

loop_cleanup:
		HxStr_del(line);

		hx_seq_iter_t *it = HxSeqIter_new(unfiltered_bits);
		while(HxSeqIter_next(it))
		{
			HxStr_del(HxSeqIter_get(it));
		}
		HxSeqIter_del(it);

		HxSeq_del(unfiltered_bits);
		HxSeq_del(bits);
	}

	if (version_nr == self->tokens->empty)
	{
		printf("%s: No version string encountered.", HxExc_get_error_name(HxValueError));
		exit(HxValueError);
	}

	hx_str_t *joined_source = HxStr_join(self->tokens->empty, source);
	hx_glshader_t *shader = HxGLShader_new(filepath, joined_source, type, version_nr, vars);

	HxSeq_append(program->shaders, shader);

	/* final clean up */
	hx_seq_iter_t *source_iter = HxSeqIter_new(source);
	while(HxSeqIter_next(source_iter))
	{
		HxStr_del((hx_str_t *)HxSeqIter_get(source_iter));
	}
	HxSeqIter_del(source_iter);

	HxFileIter_del(lines);

	HxStr_del(joined_source);
}


int
HxShaderRegistry_get_var_type_by_name(hx_shader_registry_t *self, hx_str_t *typename)
{
	int type;
	if(HxStr_equals(typename, self->tokens->vec2))
	{
		return HX_GL_VARTYPE_VEC2;
	}
	else if (HxStr_equals(typename, self->tokens->vec3))
	{
		return HX_GL_VARTYPE_VEC3;
	}
	else if (HxStr_equals(typename, self->tokens->vec4))
	{
		return HX_GL_VARTYPE_VEC4;
	}
	else if (HxStr_equals(typename, self->tokens->mat3))
	{
		return HX_GL_VARTYPE_MAT3;
	}
	else if (HxStr_equals(typename, self->tokens->mat4))
	{
		return HX_GL_VARTYPE_MAT4;
	}
	else if (HxStr_equals(typename, self->tokens->sampler1d))
	{
		type = HX_GL_VARTYPE_SAMPLER1D;
	}
	else if (HxStr_equals(typename, self->tokens->sampler2d))
	{
		return HX_GL_VARTYPE_SAMPLER2D;
	}
	else if (HxStr_equals(typename, self->tokens->sampler3d))
	{
			type = HX_GL_VARTYPE_SAMPLER3D;
	}
	else
	{
		hx_str_t *joined_typenames = HxStr_join(self->tokens->comma, self->type_tokens);
		printf("%s: Expected one of [%s] after 'in' token, got none.",
				HxExc_get_error_name(HxValueError), joined_typenames->s);
		HxStr_del(joined_typenames);
		exit(HxValueError);
	}
	return -1;
}
