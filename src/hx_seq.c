#include<stddef.h>
#include<stdarg.h>
#include<stdbool.h>

#include "hx_exc.h"
#include "hx_seq.h"
#include "hx_str.h"
#include "hx_utils.h"


hx_seq_t *
HxSeq_new(void *first, ...)
{
	va_list args;
	hx_seq_t *hxseq = (hx_seq_t *)Hx_Utils_allocate(sizeof(hx_seq_t));

	if (first == NULL)
	{
		hxseq->first = NULL;
		hxseq->length = 0;
		return hxseq;
	}
	/* construct first element by hand to avoid infinite
	 * loop when calling HxSeq_append here
	 */
	else
	{
		hxseq->first = HxSeqElem_new(first, NULL);
		hxseq->length = 1;
	}

	void *obj;
	long len = 0;
	va_start(args, first);
	while(1)
	{
		obj = va_arg(args, void *);
		if (obj == NULL)
		{
			break;
		}
		HxSeq_append(hxseq, obj);
		len++;
	}

	va_end(args);
	return hxseq;
}


void
HxSeq_del(hx_seq_t *self)
{
	hx_seq_elem_t *current = self->first;
	hx_seq_elem_t *tmp;
	while(current != NULL)
	{
		tmp = current->next;
		HxSeqElem_del(current);
		current = tmp;
	}
	free(self);
}


void
HxSeq_append(hx_seq_t *self, void *obj)
{
	hx_seq_t *seq = HxSeq_new(obj, NULL);
	HxSeq_extend_base(
			self,
			false,
			HxSeq_len(self) - 1,
			seq);
	HxSeq_del(seq);
}


hx_seq_t *
HxSeq_add(hx_seq_t *self, hx_seq_t *other)
{
	hx_seq_t *newseq = HxSeq_new(NULL);
	HxSeq_extend(newseq, self);
	HxSeq_extend(newseq, other);
	return newseq;
}


void
HxSeq_extend(hx_seq_t *self, hx_seq_t *other)
{
	HxSeq_extend_base(self, false, HxSeq_len(self) - 1, other);
}


/*
 * Internal use. Extend the sequence *self* *before* or after *index*
 * by *other* sequence.
 *
 * N.B. Look before you leap. An invalid index will terminate the program
 * if and only if HxSeq_len(self) != 0.
 */
void
HxSeq_extend_base(hx_seq_t *self, bool before, long index, hx_seq_t *other)
{
	hx_seq_elem_t *before_elem = NULL, *after_elem = NULL, *curr_elem = NULL;

	if (HxSeq_len(self) != 0)
	{
		HxSeq_check_index(self, index);

		if (before)
		{
			before_elem = HxSeq_get_elem(self, index - 1);
			after_elem = before_elem->next;
		}
		else
		{
			before_elem = HxSeq_get_elem(self, index);
			after_elem = before_elem->next;
		}
	}

	curr_elem = before_elem;
	hx_seq_iter_t * it = HxSeqIter_new(other);
	hx_seq_elem_t *el;
	if (curr_elem == NULL)
	{
		if (HxSeqIter_next(it))
		{
			el = HxSeqIter_get_elem(it);
			self->first = HxSeqElem_new(el->obj, NULL);
			curr_elem = self->first;
			self->length++;
		}
	}

	while(HxSeqIter_next(it))
	{
		el = HxSeqIter_get_elem(it);
		curr_elem->next = HxSeqElem_new(el->obj, NULL);
		curr_elem = curr_elem->next;
		self->length++;
	}
	HxSeqIter_del(it);

	if (HxSeq_len(self) > 1)
	{
		curr_elem = HxSeq_get_elem(self, HxSeq_len(self) - 1);
		curr_elem->next = after_elem;
	}
}


/*
 * Look before you leap. Invalid index terminates the program.
 */
void
HxSeq_insert(hx_seq_t *self, long index, void *obj)
{
	hx_seq_t *seq = HxSeq_new(obj, NULL);
	HxSeq_extend_base(self, true, index, seq);
	HxSeq_del(seq);
}


/*
 * Look before you leap. Invalid index terminates the program.
 */
void *
HxSeq_pop(hx_seq_t *self, long index)
{
	/* no invalid indices past this point. */
	HxSeq_check_index(self, index);

	void *rv = NULL;
	hx_seq_elem_t *prev = NULL, *current = NULL;
	hx_seq_iter_t *it = HxSeqIter_new(self);
	long i = 0;
	while(i < index)
	{
		HxSeqIter_next(it);
		prev = HxSeqIter_get_elem(it);
		i++;
	}

	if (prev == NULL)
	{
		if (HxSeqIter_next(it))
		{
			current = HxSeqIter_get_elem(it);
			rv = current->obj;
			HxSeqIter_next(it);

			/* could be actual element or NULL.
			 * case NULL => sequence is set to empty
			 */
			self->first = HxSeqIter_get_elem(it);
		}
		else
		{
			/* index doesn't exist, should not happen */
		}
	}
	else
	{
		if (HxSeqIter_next(it))
		{
			current = HxSeqIter_get_elem(it);
			rv = current->obj;
			HxSeqIter_next(it);

			/* could be actual element or NULL.
			 * case NULL => end of sequence
			 */
			prev->next = HxSeqIter_get_elem(it);
		}
		else
		{
			/* index doesn't exist, should not happen */
		}
	}
	self->length--;
	HxSeqElem_del(current);
	HxSeqIter_del(it);
	return rv;
}


long
HxSeq_len(hx_seq_t *self)
{
	long i = 0;
	hx_seq_iter_t *it = HxSeqIter_new(self);
	while(HxSeqIter_next(it))
	{
		i++;
	}
	HxSeqIter_del(it);
	return i;
}


long
HxSeq_index(hx_seq_t *self, void *obj)
{
	long rv = -1;
	hx_seq_iter_t *it = HxSeqIter_new(self);
	long i = 0;
	while(HxSeqIter_next(it))
	{
		if (HxSeqIter_get(it) == obj)
		{
			rv = i;
			break;
		}
		i++;
	}
	HxSeqIter_del(it);
	return rv;
}


/*
 * Look before you leap. Invalid index terminates program.
 */
void *
HxSeq_get_item(hx_seq_t *self, long index)
{
	HxSeq_check_index(self, index);

	hx_seq_elem_t *elem = HxSeq_get_elem(self, index);
	return elem->obj;
}


void HxSeq_check_index(hx_seq_t *self, long index)
{
	long len = HxSeq_len(self);
	if (len != 0)
	{
		if (index < 0 || index > len - 1)
		{
			printf("%s: %li\n", HxExc_get_error_name(HxIndexError), index);
			exit (HxIndexError);
		}
	}
}

hx_seq_elem_t *
HxSeq_get_elem(hx_seq_t *self, long index)
{
	HxSeq_check_index(self, index);

	hx_seq_elem_t *elem = NULL;
	hx_seq_iter_t *it = HxSeqIter_new(self);
	long i = 0;
	while(HxSeqIter_next(it))
	{
		if (i == index)
		{
			elem = HxSeqIter_get_elem(it);
			break;
		}
		i++;
	}
	HxSeqIter_del(it);
	return elem;
}


/*
 * In place sorting. Inefficient.
 */
void
HxSeq_sort(hx_seq_t *self, void *((* algo_fn)(hx_seq_t *sortee, int (* cmp)(void *, void *))), int (* comp_fn)(void *, void *))
{
	if (HxSeq_len(self) == 0)
	{
		return;
	}

	hx_seq_t *clone = HxSeq_clone(self);
	hx_seq_t *sorted = (hx_seq_t *)algo_fn(clone, comp_fn);

	hx_seq_iter_t *it = HxSeqIter_new(sorted);
	while(HxSeqIter_next(it))
	{
		HxSeq_insert(self, 0, HxSeq_pop(self, HxSeq_index(self, HxSeqIter_get(it))));
	}
	HxSeqIter_del(it);
	HxSeq_del(clone);
}


/*
 * More efficient than in place sorting in HxSeq_sort.
 *
 * @return Newly created sequence of sorted elements from *self*.
 */
hx_seq_t *
HxSeq_sorted(hx_seq_t *self)
{
	hx_seq_t *clone = HxSeq_clone(self);
	hx_seq_t *res = (hx_seq_t *)Hx_Utils_quicksort(clone, HxStr_compare);
	HxSeq_del(clone);
	return res;
}


/*
 * In-place rerversal of the sequence's order.
 */
void
HxSeq_reverse(hx_seq_t *self)
{

}


hx_seq_t *
HxSeq_filter(hx_seq_t *self, bool (*filter)(void *))
{
	hx_seq_t *filtered = HxSeq_new(NULL);
	hx_seq_iter_t *it = HxSeqIter_new(self);
	while(HxSeqIter_next(it))
	{
		if (filter(HxSeqIter_get(it)))
		{
			HxSeq_append(filtered, HxSeqIter_get(it));
		}
	}
	HxSeqIter_del(it);
	return filtered;
}


/*
 * Make a slice of the passed in hx_seq_t.
 *
 * @param self A pointer to a hx_seq_t object.
 * @param start The index to the node on *self* that will become the first node in the new hx_seq_t.
 * @param stop The index (exclusive) to the node on *self* that will become the last node on the new hx_seq_t.
 * @param step Hops between start and stop nodes. If unequal to one, the last node on the new hx_seq_t might
 * 				not become the *stop* node from *self*.
 *
 * @return A pointer to a new hx_seq_t which is a slice of the passed in hx_seq_t.
 */
hx_seq_t *
HxSeq_slice(hx_seq_t *self, long start, long stop, long step)
{
	if (step < 1)
	{
		printf("%s: 'step' must not be less than 1.", HxExc_get_error_name(HxValueError));
		exit(HxValueError);
	}

	hx_seq_t *newseq = HxSeq_new(NULL);

	/* return empty sequence if *start* is bigger than length of sequence */
	if (HxSeq_len(self) < start)
	{
		return newseq;
	}

	hx_seq_iter_t *it = HxSeqIter_new(self);

	/* advance iterator until *start* */
	long indexcount = 0;
	while(indexcount < start)
	{
		if (!HxSeqIter_next(it))
		{
			break;
		}
		indexcount++;
	}

	long i;
	bool keep_going;
	for (i=start; i < stop; i+=step)
	{
		/* advance by one */
		if (!HxSeqIter_next(it))
		{
			break;
		}

		HxSeq_append(newseq, HxSeqIter_get(it));

		/* advance the index on the iterator by *step* - 1,
		 * so it stands at one click before the next wanted
		 * index.
		 */
		keep_going = true;
		int stepcount = 0;
		while(stepcount < step - 1)
		{
			if (!HxSeqIter_next(it))
			{
				keep_going = false;
				break;
			}
			stepcount++;
		}
		if (!keep_going)
		{
			break;
		}
	}

	HxSeqIter_del(it);
	return newseq;
}


hx_seq_t *
HxSeq_clone(hx_seq_t *self)
{
	return HxSeq_slice(self, 0, HxSeq_len(self), 1);
}


hx_seq_elem_t *
HxSeqElem_new(void *obj, void *next)
{
	hx_seq_elem_t *el = (hx_seq_elem_t *)Hx_Utils_allocate(sizeof(hx_seq_elem_t));
	el->obj = obj;
	el->next = next;
	return el;
}


void
HxSeqElem_del(hx_seq_elem_t *self)
{
	free(self);
}


hx_seq_elem_t *
HxSeqElem_clone(hx_seq_elem_t *self)
{
	return HxSeqElem_new(self->obj, self->next);
}


hx_seq_iter_t *
HxSeqIter_new(hx_seq_t *seq)
{
	hx_seq_iter_t *it = (hx_seq_iter_t *)Hx_Utils_allocate(sizeof(hx_seq_iter_t));
	it->seq = seq;
	it->initiated = false;
	return it;
}


bool HxSeqIter_next(hx_seq_iter_t *self)
{
	if (!self->initiated)
	{
		self->current = self->seq->first;
		self->initiated = true;
	}
	else
	{
		self->current = (self->current == NULL ? NULL : self->current->next);
	}
	return self->current != NULL;
}


void
HxSeqIter_rewind(hx_seq_iter_t *self)
{
	self->current = NULL;
	self->initiated = false;
}


void *
HxSeqIter_get(hx_seq_iter_t *self)
{
	return self->current->obj;
}


hx_seq_elem_t *
HxSeqIter_get_elem(hx_seq_iter_t *self)
{
	return self->current;
}


void
HxSeqIter_del(hx_seq_iter_t *self)
{
	free(self);
}
