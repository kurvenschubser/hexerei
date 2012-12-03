#ifndef HX_GLVAR_H
#define HX_GLVAR_H

#include "hx_shaderregistry.h"


/* The hx_glvar_t represents a single variable in a
 * hx_glshader_t instance.
 *
 * Usage:
 * 		Please see hx_shader_registry_t doc string.
 */
typedef struct
{
	/* this variable's open gl handle */
	GLuint id;

	/* this variable's name as parsed from the glsl source
	 *  code.
	 */
	hx_str_t *name;

	/* this variable's type as parsed from the glsl source
	 * code. must be one of 'Hx_GlVartype'.
	 */
	int type;

	/* this variable's position in the input buffer */
	int position;

	bool is_in;
	bool is_uniform;

} hx_glvar_t;


hx_glvar_t *
HxGLVar_new(hx_str_t *name, int type, int position, bool is_in, bool is_uniform);


void
HxGLVar_del(hx_glvar_t *self);

#endif /* HX_GLVAR_H */
