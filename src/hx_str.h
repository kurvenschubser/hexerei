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
} HxStr;


HxStr *
HxStr_new(const char *s);

long
HxStr_len(HxStr *self);

HxStr *
HxStr_join(HxStr *self, HxSeq *seq);

HxStr *
HxStr_add(HxStr *self, HxStr *other);

#endif		/* HX_STR_H */
