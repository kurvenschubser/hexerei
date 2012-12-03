#ifndef HX_GLSHADER_H
#define HX_GLSHADER_H


#include "hx_shaderregistry.h"
#include "hx_glvar.h"
#include "hx_utils.h"


/* The hx_glshader_t represents a sub program of an OpenGL
 * program.
 *
 * Usage:
 * 		Please see hx_shader_registry_t doc string.
 *
 */
typedef struct
{
	/* the open gl handle to this shader program */
	GLuint id;

	/* a string storing the filename of the shader progam
	 * source code.
	 */
	hx_str_t *filename;

	/* a string storing the glsl source code of the
	 * shader program.
	 */
	hx_str_t *source;

	/* the shader type, one of [GL_VERTEX_SHADER,
	 * GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER].
	 */
	int type;

	/* the glsl version string */
	hx_str_t *version;

	/* a sequence of hx_glvar_t with the variable
	 * representations for this shader
	 */
	hx_seq_t *vars;

	bool is_compiled;

} hx_glshader_t;


hx_glshader_t *
HxGLShader_new(hx_str_t *filename, hx_str_t *source, int type, hx_str_t *version, hx_seq_t *vars);


void
HxGLShader_del(hx_glshader_t *self);


bool
HxGLShader_is_compiled(hx_glshader_t *self);


void
HxGLShader_compile(hx_glshader_t *self);


hx_str_t *
HxGLShader_get_typename(hx_glshader_t *self);

#endif /* HX_GLSHADER_H */


