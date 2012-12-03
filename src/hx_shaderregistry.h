#ifndef HX_SHADERREGISTRY_H
#define HX_SHADERREGISTRY_H

#include <glew.h>
#include "hx_str.h"
#include "hx_glprogram.h"


enum Hx_GlVartype{
	HX_GL_VARTYPE_NONE = 0,
	HX_GL_VARTYPE_VEC2,
	HX_GL_VARTYPE_VEC3,
	HX_GL_VARTYPE_VEC4,
	HX_GL_VARTYPE_MAT3,
	HX_GL_VARTYPE_MAT4,
	HX_GL_VARTYPE_SAMPLER1D,
	HX_GL_VARTYPE_SAMPLER2D,
	HX_GL_VARTYPE_SAMPLER3D
};


struct HxParserTokens
{
	hx_str_t *blank;
	hx_str_t *empty;
	hx_str_t *main;
	hx_str_t *void_;
	hx_str_t *layout;
	hx_str_t *uniform;
	hx_str_t *bracket_open;
	hx_str_t *bracket_close;
	hx_str_t *eq;
	hx_str_t *comma;
	hx_str_t *semicolon;
	hx_str_t *version;
	hx_str_t *in;
	hx_str_t *out;
	hx_str_t *vec2;
	hx_str_t *vec3;
	hx_str_t *vec4;
	hx_str_t *mat3;
	hx_str_t *mat4;
	hx_str_t *sampler1d;
	hx_str_t *sampler2d;
	hx_str_t *sampler3d;
};


/* The hx_shader_registry_t does most of the setting up
 * and book keeping of the gl programs registered with
 * it.
 *
 * Usage:
 * 		hx_shader_registry *reg = HxShaderRegistry_new();
 *
 * 		hx_str_t *path = HxStr_new("/path/to/my/kickass-glprogram");
 * 		HxShaderRegistry_load(reg, path);
 *
 * 		hx_str_t *program_name = HxStr_new("kickass-glprogram");
 * 		hx_glprogram_t *program = HxShaderRegistry_use(reg, program_name);
 *
 * 		HxGLProgram_set_input(program, input);
 * 		HxGLProgram_use(program);
 *
 * 		etc.
 */
typedef struct
{
	hx_seq_t *items;
	hx_seq_t *type_tokens;
	struct HxParserTokens *tokens;
} hx_shader_registry_t;


hx_shader_registry_t *
HxShaderRegistry_new(void);


void
HxShaderRegistry_del(hx_shader_registry_t *self);


hx_glprogram_t *
HxShaderRegistry_register(hx_shader_registry_t *self, hx_glprogram_t *shader);


void
HxShaderRegistry_unregister(hx_shader_registry_t *self, hx_str_t *name);


void
HxShaderRegistry_load(hx_shader_registry_t *self, hx_str_t *folder);


hx_glprogram_t *
HxShaderRegistry_get(hx_shader_registry_t *self, hx_str_t *name);


hx_glprogram_t *
HxShaderRegistry_get_by_index(hx_shader_registry_t *self, int index);


/*
 * Get sequence iter of pointers to hx_str_t .
 */
hx_seq_t *
HxShaderRegistry_get_keys(hx_shader_registry_t *self);


/*
 * Get sequence iter of pointers to hx_shader_t.
 */
hx_seq_iter_t *
HxShaderRegistry_get_values(hx_shader_registry_t *self);


void
HxShaderRegistry_parse(hx_shader_registry_t *self, hx_glprogram_t *shader, hx_str_t *filepath);


int
HxShaderRegistry_get_var_type_by_name(hx_shader_registry_t *self, hx_str_t *typename);

#endif /* HX_SHADERREGISTRY_H */
