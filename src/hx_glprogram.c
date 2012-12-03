#include "hx_exc.h"
#include "hx_glprogram.h"
#include "hx_glshader.h"
#include "hx_shaderregistry.h"
#include "hx_str.h"



hx_glprogram_t *
HxGLProgram_new(hx_str_t *name)
{
	hx_glprogram_t *self = (hx_glprogram_t *)Hx_Utils_allocate(sizeof(hx_glprogram_t));
	self->id = 0;
	self->name = name;

	self->shaders = HxSeq_new(NULL);

	self->is_compiled = false;

	return self;
}


void
HxGLProgram_del(hx_glprogram_t *self)
{

	HxStr_del(self->name);

	hx_seq_iter_t *it;
	it = HxSeqIter_new(self->shaders);
	hx_glshader_t *shader;
	while(HxSeqIter_next(it))
	{
		shader = (hx_glshader_t *)HxSeqIter_get(it);
		glDetachShader(self->id, shader->id);
		HxGLShader_del(shader);
	}
	HxSeqIter_del(it);
	HxSeq_del(self->shaders);

	free(self);
}


long
HxGLProgram_len(hx_glprogram_t *self)
{
	return HxSeq_len(self->shaders);
}


bool
HxGLProgram_is_compiled(hx_glprogram_t *self)
{
	return self->is_compiled;
}


void
HxGLProgram_compile(hx_glprogram_t *self)
{
	GLenum errval;

	self->id = glCreateProgram();

	errval = glGetError();
	if (errval != GL_NO_ERROR)
	{
		printf("%s: %s\n", HxExc_get_error_name(HxGLError), gluErrorString(errval));
		exit(HxGLError);
	}

	hx_seq_iter_t *shaders = HxSeqIter_new(self->shaders);
	hx_glshader_t *shader;
	while(HxSeqIter_next(shaders))
	{
		errval = GL_NO_ERROR;

		shader = (hx_glshader_t *)HxSeqIter_get(shaders);

		if (!HxGLShader_is_compiled(shader))
		{
			HxGLShader_compile(shader);
		}
		glAttachShader(self->id, shader->id);

		errval = glGetError();
		if (errval != GL_NO_ERROR)
		{
			printf("%s: %s\n", HxExc_get_error_name(HxGLError), gluErrorString(errval));
			exit(HxGLError);
		}
	}

	glLinkProgram(self->id);

	GLint status;
	glGetProgramiv (self->id, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint loginfo_len;
		GLchar *loginfo;

		glGetProgramiv(self->id, GL_INFO_LOG_LENGTH, &loginfo_len);

		loginfo = (GLchar *)Hx_Utils_allocate((loginfo_len + 1) * sizeof(GLchar));
		glGetProgramInfoLog(self->id, loginfo_len, NULL, loginfo);
		fprintf(stderr, "%s: Linker failure: %s\n", HxExc_get_error_name(HxGLError), loginfo);
		free(loginfo);
	}

	errval = glGetError();
	if (errval != GL_NO_ERROR)
	{
		printf("%s: %s\n", HxExc_get_error_name(HxGLError), gluErrorString(errval));
		exit(HxGLError);
	}

	self->is_compiled = true;
}


void
HxGLProgram_use(hx_glprogram_t *self)
{
	if (!HxGLProgram_is_compiled(self))
	{
		HxGLProgram_compile(self);
	}
	glUseProgram(self->id);

}
