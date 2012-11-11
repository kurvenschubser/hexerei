#ifndef HX_SEQ_H
#define HX_SEQ_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>



typedef struct HxSeqElem_
{
	struct HxSeqElem_ *next;
	void *obj;
} HxSeqElem;


typedef struct HxSeq_
{
	HxSeqElem *first;
	long length;
} HxSeq;


typedef struct HxSeqIter_
{
	HxSeq *seq;
	HxSeqElem *current;
	bool initiated;
} HxSeqIter;


HxSeqIter *
HxSeqIter_new(HxSeq *seq);

bool HxSeqIter_next(HxSeqIter *self);

void
HxSeqIter_rewind(HxSeqIter *self);

void *
HxSeqIter_get(HxSeqIter *self);

void
HxSeqIter_del(HxSeqIter *self);

HxSeq *
HxSeq_new(void *first, ...);

void
HxSeq_del(HxSeq *self);

void
HxSeq_append(HxSeq *self, void *obj);

void *
HxSeq_pop(HxSeq *self, long index);

long
HxSeq_len(HxSeq *self);

HxSeqElem *
HxSeqElem_new(void *obj, void *next);

void
HxSeqElem_del(HxSeqElem *self);

#endif		/* HX_SEQ_H */
