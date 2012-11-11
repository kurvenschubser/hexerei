#include<stddef.h>
#include<stdarg.h>
#include<stdbool.h>

#include "hx_seq.h"


HxSeqIter *
HxSeqIter_new(HxSeq *seq)
{
	HxSeqIter *it = malloc(sizeof(HxSeqIter));
	if (it == NULL)
	{
		printf("MemoryAllocationError");
		return NULL;
	}
	it->seq = seq;
	it->initiated = false;
	return it;
}


bool HxSeqIter_next(HxSeqIter *self)
{
	if (!self->initiated)
	{
		self->current = self->seq->first;
		self->initiated = true;
	}
	else
	{
		self->current = self->current->next;
	}
	return self->current != NULL;
}


void
HxSeqIter_rewind(HxSeqIter *self)
{
	self->current = NULL;
	self->initiated = false;
}


void *
HxSeqIter_get(HxSeqIter *self)
{
	return self->current->obj;
}


HxSeqElem *
HxSeqIter_get_seq_elem(HxSeqIter *self)
{
	return self->current;
}


void
HxSeqIter_del(HxSeqIter *self)
{
	free(self);
}


HxSeq *
HxSeq_new(void *first, ...)
{
	va_list args;
	HxSeq *hxseq = malloc(sizeof(HxSeq));
	if (hxseq == NULL)
	{
		printf("MemoryAllocationError");
		return NULL;
	}

	hxseq->first = NULL;
	hxseq->length = 0;

	if (first == NULL)
	{
		return hxseq;
	}

	HxSeq_append(hxseq, first);

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
HxSeq_del(HxSeq *self)
{
	HxSeqElem *current = self->first;
	HxSeqElem *tmp;
	while(current != NULL)
	{
		tmp = current->next;
		HxSeqElem_del(current);
		current = tmp;
	}
	free(self);
}


void
HxSeq_append(HxSeq *self, void *obj)
{
	HxSeqElem *next = HxSeqElem_new(obj, NULL);

	/* initialize with first element */
	if (self->first == NULL)
	{
		self->first = next;
	}
	else
	{
		HxSeqElem *current = self->first;

		/* get last element */
		while(1)
		{

			if (current->next == NULL)
			{
				break;
			}
			current = current->next;
		}
		current->next = next;
	}

	self->length++;
}


void *
HxSeq_pop(HxSeq *self, long index)
{
	void *rv = NULL;
	HxSeqElem *current;
	HxSeqIter *it = HxSeqIter_new(self);
	long i = 0;
	while(HxSeqIter_next(it))
	{
		current = HxSeqIter_get_seq_elem(it);
		if (i == index)
		{
			self->first = current->next;
			self->length--;
			rv = current->obj;
			HxSeqElem_del(current);
		}
		i++;
	}

	if (rv == NULL)
	{
		printf("IndexError");
	}

	HxSeqIter_del(it);
	return rv;
}


long
HxSeq_len(HxSeq *self)
{
	return self->length;
}


HxSeqElem *
HxSeqElem_new(void *obj, void *next)
{
	HxSeqElem *el = malloc(sizeof(HxSeqElem));
	if (el == NULL)
	{
		printf("MemoryAllocationError");
		return NULL;
	}
	el->obj = obj;
	el->next = next;
	return el;
}


void
HxSeqElem_del(HxSeqElem *self)
{
	free(self);
}
