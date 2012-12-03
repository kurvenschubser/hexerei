#include "hx_glvar.h"


hx_glvar_t *
HxGLVar_new(hx_str_t *name, int type, int position, bool is_in, bool is_uniform)
{
	hx_glvar_t *self = (hx_glvar_t *)Hx_Utils_allocate(sizeof(hx_glvar_t));
	self->name = name;
	self->type = type;
	self->position = position;
	self->is_in = is_in;
	self->is_uniform = is_uniform;
	return self;
}


void
HxGLVar_del(hx_glvar_t *self)
{
	HxStr_del(self->name);
	free(self);
}
