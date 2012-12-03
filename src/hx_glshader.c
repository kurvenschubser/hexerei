#include "hx_exc.h"
#include "hx_glshader.h"


hx_glshader_t *
HxGLShader_new(hx_str_t *filename, hx_str_t *source, int type, hx_str_t *version, hx_seq_t *vars)
{
	hx_glshader_t *self = (hx_glshader_t *)Hx_Utils_allocate(sizeof(hx_glshader_t));

	self->id = 0;
	self->filename = filename;
	self->source = source;
	self->type = type;
	self->version = version;
	self->vars = vars;
	self->is_compiled = false;
	return self;
}


void
HxGLShader_del(hx_glshader_t *self)
{
	glDeleteShader(self->id);

	HxStr_del(self->filename);
	HxStr_del(self->source);
	HxStr_del(self->version);

	hx_seq_iter_t *vars_iter = HxSeqIter_new(self->vars);
	while(HxSeqIter_next(vars_iter))
	{
		HxGLVar_del((hx_glvar_t *)HxSeqIter_get(vars_iter));
	}
	HxSeqIter_del(vars_iter);
	HxSeq_del(self->vars);

	free(self);
}


bool
HxGLShader_is_compiled(hx_glshader_t *self)
{
	return self->is_compiled;
}


void
HxGLShader_compile(hx_glshader_t *self)
{
	glGetError();		/* pop last error */

	self->id = glCreateShader(self->type);
	if (self->id == 0)
	{
		const GLenum errval = glGetError();
		printf("%s: %s\n", HxExc_get_error_name(HxGLError), gluErrorString(errval));
		exit(HxGLError);
	}

	const GLchar **sources = { ((hx_str_t *)self->source)->s };

	glShaderSource(self->id, 1, sources, NULL);
	glCompileShader(self->id);

	GLint status;
	glGetShaderiv(self->id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint loginfo_len;
		GLchar *loginfo;

		glGetShaderiv(self->id, GL_INFO_LOG_LENGTH, &loginfo_len);

		loginfo = (GLchar *)Hx_Utils_allocate((loginfo_len + 1) * sizeof(GLchar));
		glGetShaderInfoLog(self->id, loginfo_len, NULL, loginfo);

		hx_str_t *typename = HxGLShader_get_typename(self);
		fprintf(stderr, "%s: Compile failure in %s shader: %s\n",
				HxExc_get_error_name(HxGLError), typename->s, loginfo);

		HxStr_del(typename);
		free(loginfo);

		exit(HxGLError);
	}

	self->is_compiled = true;
}


hx_str_t *
HxGLShader_get_typename(hx_glshader_t *self)
{
	const char *typename = NULL;
	switch(self->type)
	{
		case GL_VERTEX_SHADER:
			typename = "vertex";
			break;
		case GL_GEOMETRY_SHADER:
			typename = "geometry";
			break;
		case GL_FRAGMENT_SHADER:
			typename = "fragment";
			break;
		default:
			fprintf(stderr, "%s: unknown shader type: %d", HxExc_get_error_name(HxValueError), self->type);
			exit(HxValueError);
	}
	return HxStr_new(typename);
}


