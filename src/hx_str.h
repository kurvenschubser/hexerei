#ifndef HX_STR_H
#define HX_STR_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "hx_seq.h"


typedef struct HxStr_
{
	const char *s;
	long length;
} hx_str_t;


typedef struct HxStrIter_
{
	hx_str_t *str;
	long current;
	bool initiated;
} hx_str_iter_t;


hx_str_t *
HxStr_new(const char *s);


void
HxStr_del(hx_str_t *self);


long
HxStr_len(hx_str_t *self);


hx_str_t *
HxStr_join(hx_str_t *self, hx_seq_t *seq);


hx_seq_t *
HxStr_split(hx_str_t *self, hx_str_t *divider);


bool
HxStr_startswith(hx_str_t *self, hx_str_t *what);


bool
HxStr_endswith(hx_str_t *self, hx_str_t *what);


bool
HxStr_contains(hx_str_t *self, hx_str_t *what);


int
HxStr_search_base(hx_str_t *self, hx_str_t *what, int offset);


int
HxStr_index(hx_str_t *self, hx_str_t *what, int offset);


hx_str_t *
HxStr_add(hx_str_t *self, hx_str_t *other);


bool
HxStr_equals(hx_str_t *self, hx_str_t *other);

int
HxStr_compare(const void *self, const void *other);


hx_str_t *
HxStr_trim(hx_str_t *self);


hx_str_t *
HxStr_replace(hx_str_t *self, hx_str_t *what, hx_str_t *with);


hx_str_t *
HxStr_slice(hx_str_t *self, long start, long stop, long step);


hx_str_t *
HxStr_from_int(int d);


hx_str_t *
HxStr_from_long(long d);


hx_str_t *
HxStr_from_float(float d, int precision);


hx_str_t *
HxStr_from_double(double d, int precision);


long
HxStr_to_long(hx_str_t *self, int base);


bool
HxStr_to_bool(void *self);


hx_str_iter_t *
HxStrIter_new(hx_str_t *seq);


bool
HxStrIter_next(hx_str_iter_t *self);


void
HxStrIter_rewind(hx_str_iter_t *self);


hx_str_t *
HxStrIter_get(hx_str_iter_t *self);


void
HxStrIter_del(hx_str_iter_t *self);

#endif		/* HX_STR_H */
