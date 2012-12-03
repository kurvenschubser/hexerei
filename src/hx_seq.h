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
} hx_seq_elem_t;


typedef struct HxSeq_
{
	hx_seq_elem_t *first;
	long length;
} hx_seq_t;


typedef struct HxSeqIter_
{
	hx_seq_t *seq;
	hx_seq_elem_t *current;
	bool initiated;
} hx_seq_iter_t;


hx_seq_t *
HxSeq_new(void *first, ...);


void
HxSeq_del(hx_seq_t *self);


void
HxSeq_append(hx_seq_t *self, void *obj);


/*
 * @return A new sequence made up of all elements of *self* and *other*.
 */
hx_seq_t *
HxSeq_add(hx_seq_t *self, hx_seq_t *other);


/*
 * In-place adding off all elements from other to *self*.
 */
void
HxSeq_extend(hx_seq_t *self, hx_seq_t *other);


void
HxSeq_extend_base(hx_seq_t *self, bool before, long index, hx_seq_t *other);


hx_seq_t *
HxSeq_clone(hx_seq_t *self);


void *
HxSeq_pop(hx_seq_t *self, long index);


long
HxSeq_len(hx_seq_t *self);


long
HxSeq_index(hx_seq_t *self, void *obj);


void *
HxSeq_get_item(hx_seq_t *self, long index);


hx_seq_elem_t *
HxSeq_get_elem(hx_seq_t *self, long index);


void HxSeq_check_index(hx_seq_t *self, long index);


void
HxSeq_sort(hx_seq_t *self, void *((* algo_fn)(hx_seq_t *sortee, int (* cmp)(void *, void *))), int (* comp_fn)(void *, void *));


hx_seq_t *
HxSeq_sorted(hx_seq_t *self);


void
HxSeq_reverse(hx_seq_t *self);


hx_seq_t *
HxSeq_filter(hx_seq_t *self, bool (*filter)(void *));


hx_seq_t *
HxSeq_slice(hx_seq_t *self, long start, long stop, long step);


hx_seq_elem_t *
HxSeqElem_new(void *obj, void *next);


void
HxSeqElem_del(hx_seq_elem_t *self);


hx_seq_elem_t *
HxSeqElem_clone(hx_seq_elem_t *self);


hx_seq_iter_t *
HxSeqIter_new(hx_seq_t *seq);


bool
HxSeqIter_next(hx_seq_iter_t *self);


void
HxSeqIter_rewind(hx_seq_iter_t *self);


void *
HxSeqIter_get(hx_seq_iter_t *self);


hx_seq_elem_t *
HxSeqIter_get_elem(hx_seq_iter_t *self);


void
HxSeqIter_del(hx_seq_iter_t *self);

#endif		/* HX_SEQ_H */
