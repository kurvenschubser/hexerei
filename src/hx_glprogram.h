#ifndef HX_GLPROGRAM_H
#define HX_GLPROGRAM_H

#include <glew.h>
#include <GL/freeglut.h>

#include "hx_seq.h"
#include "hx_str.h"
#include "hx_path.h"
#include "hx_utils.h"


/*
 * The hx_glprogram_t type represents an OpenGl 'program'.
 * It stores the individual shader programs that will
 * be used to 'glLinkProgram'.
 *
 * The hx_glprogram_t is tightly coupled to the
 * hx_shader_registry_t which handles most of its setup.
 *
 * Usage:
 * 		Please see hx_shader_registry_t doc string.
 *
 */
typedef struct
{
	hx_str_t *name;

	/* the OpenGL handle of the program */
	GLuint id;

	/* the sequence of hx_glshader_t objects this gl program is made of. */
	hx_seq_t *shaders;

	bool is_compiled;

} hx_glprogram_t;


hx_glprogram_t *
HxGLProgram_new();


void
HxGLProgram_del(hx_glprogram_t *self);


long
HxGLProgram_len(hx_glprogram_t *self);


bool
HxGLProgram_is_compiled(hx_glprogram_t *self);


void
HxGLProgram_compile(hx_glprogram_t *self);


void
HxGLProgram_use(hx_glprogram_t *self);


void
HxGLProgram_configure_input(hx_glprogram_t *self, hx_str_t *varname, size_t size, int gltype, int stride, int offset);


GLuint
HxGLProgram_get_uniform_var(hx_glprogram_t *self, hx_str_t *varname);

#endif		/* HX_GLPROGRAM_H */
