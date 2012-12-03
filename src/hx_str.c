#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "hx_exc.h"
#include "hx_str.h"
#include "hx_utils.h"


hx_str_t *
HxStr_new(const char *s)
{
	hx_str_t *hxstr = (hx_str_t *)Hx_Utils_allocate(sizeof(hx_str_t));

	char *tmp = (char *)Hx_Utils_allocate((strlen(s) + 1) * sizeof(char));

	strcpy(tmp, s);
	hxstr->s = tmp;
	hxstr->length = strlen(s);

	return hxstr;
}


void
HxStr_del(hx_str_t *self)
{
	free((void *)self->s);
	free(self);
}


long
HxStr_len(hx_str_t *self)
{
	return self->length;
}


hx_str_t *
HxStr_join(hx_str_t *self, hx_seq_t *seq)
{
	if (HxSeq_len(seq) == 0)
	{
		return HxStr_new("");
	}

	hx_seq_iter_t *it = HxSeqIter_new(seq);
	hx_str_t *hxstr;
	long len = 0;
	long n = 0;
	while(HxSeqIter_next(it))
	{
		hxstr = (hx_str_t *)(HxSeqIter_get(it));
		len += HxStr_len(hxstr);
		n++;
	}

	size_t strsize = len + 1 + (n - 1) * HxStr_len(self);
	char *s = (char *)Hx_Utils_allocate(strsize * sizeof(char));
	strcpy(s, "");

	HxSeqIter_rewind(it);
	HxSeqIter_next(it);

	/* copy first string from sequence to tmp char array */
	hxstr = (hx_str_t *)(HxSeqIter_get(it));
	strcat(s, hxstr->s);

	/* for all the other strings hxstr in the sequence, copy *self* string and
	 * the hxstr to the tmp char array.*/
	while(HxSeqIter_next(it))
	{
		hxstr = (hx_str_t *)(HxSeqIter_get(it));
		strcat(s, self->s);
		strcat(s, hxstr->s);
	}
	HxSeqIter_del(it);

	hx_str_t *rv = HxStr_new(s);
	free(s);
	return rv;
}


hx_seq_t *
HxStr_split(hx_str_t *self, hx_str_t *divider)
{
	hx_seq_t *bits = HxSeq_new(NULL);
	hx_str_t *whatsleft = HxStr_new(self->s);
	int index;
	int divlen = HxStr_len(divider);

	char *tmp = (char *)Hx_Utils_allocate((HxStr_len(whatsleft) + 1) * sizeof(char));

	while(true)
	{
		index = HxStr_index(whatsleft, divider, 0);
		if (index == -1)
		{
			HxSeq_append(bits, HxStr_new(whatsleft->s));
			break;
		}

		/* reset tmp string */
		strcpy(tmp, "");

		/* copy *whatsleft* until before the occurrence of *divider* */
		strncat(tmp, whatsleft->s, index);

		/* construct hx_str from tmp and append to return sequence */
		hx_str_t *bit = HxStr_new(tmp);
		HxSeq_append(bits, bit);

		/* reset tmp string */
		strcpy(tmp, "");

		/* copy *whatsleft* from after the occurrence of *divider* until it's end. */
		strcat(tmp, whatsleft->s + index + divlen);

		/* delete old and make new instance for whatsleft */
		HxStr_del(whatsleft);
		whatsleft = HxStr_new(tmp);
	}
	HxStr_del(whatsleft);
	free(tmp);
	return bits;
}


bool
HxStr_startswith(hx_str_t *self, hx_str_t *what)
{
	return HxStr_search_base(self, what, 0) != -1;
}


bool
HxStr_endswith(hx_str_t *self, hx_str_t *what)
{
	int offset = HxStr_len(self) - HxStr_len(what);
	return HxStr_search_base(self, what, offset) != -1;
}


int
HxStr_search_base(hx_str_t *self, hx_str_t *what, int offset)
{
	int selflen = HxStr_len(self);
	int whatlen = HxStr_len(what);

	if (selflen < whatlen || offset > selflen - whatlen)
	{
		return -1;
	}

	int i;
	for(i = 0; i < whatlen ; i++)
	{
		if (self->s[offset+i] != what->s[i])
		{
			return -1;
		}
	}
	return i;
}


bool
HxStr_contains(hx_str_t *self, hx_str_t *what)
{
	return HxStr_index(self, what, 0) != -1;
}


int
HxStr_index(hx_str_t *self, hx_str_t *what, int offset)
{
	int maxruns = HxStr_len(self) - HxStr_len(what) + 1;
	int i;
	for(i = 0; i < maxruns; i++)
	{
		if (HxStr_search_base(self, what, i) != -1)
		{
			return i;
		}
	}
	return -1;
}


hx_str_t *
HxStr_add(hx_str_t *self, hx_str_t *other)
{
	hx_str_t *empty = HxStr_new("");
	hx_seq_t *seq = HxSeq_new(self, other, NULL);
	hx_str_t *rv = HxStr_join(empty, seq);
	HxStr_del(empty);
	HxSeq_del(seq);
	return rv;
}


bool
HxStr_equals(hx_str_t *self, hx_str_t *other)
{
	return strcmp(self->s, other->s) == 0;
}


int
HxStr_compare(const void *self, const void *other)
{
	return strcmp(((hx_str_t *)self)->s, ((hx_str_t *)other)->s);
}


hx_str_t *
HxStr_trim(hx_str_t *self)
{
	long len = HxStr_len(self);
	const char *start = self->s;
	const char *stop = self->s + (len - 1);

	char c = *start;
	while(c == ' ' || c == '\t' || c == '\n')
	{
		++start;
		c = *start;
	}

	c = *stop;
	while(c == ' ' || c == '\t' || c == '\n')
	{
		--stop;
		c = *stop;
	}

	long newlen = (stop - start + 1);
	newlen = newlen < 0 ? 0 : newlen;
	char *s = (char *)Hx_Utils_allocate((newlen + 1) * sizeof(char));

	strncpy(s, start, newlen);
	s[newlen] = '\0';

	hx_str_t *str = HxStr_new(s);
	free(s);
	return str;
}


hx_str_t *
HxStr_replace(hx_str_t *self, hx_str_t *what, hx_str_t *with)
{
	hx_seq_t *splitted = HxStr_split(self, what);
	hx_str_t *joined = HxStr_join(with, splitted);

	/* delete hx_str_t objects in 'splitted'. */
	hx_seq_iter_t *splitted_iter = HxSeqIter_new(splitted);
	while(HxSeqIter_next(splitted_iter))
	{
		HxStr_del(HxSeqIter_get(splitted_iter));
	}
	HxSeqIter_del(splitted_iter);

	HxSeq_del(splitted);
	return joined;
}


/*
 * Inefficient.
 */
hx_str_t *
HxStr_slice(hx_str_t *self, long start, long stop, long step)
{
	hx_str_iter_t *characters = HxStrIter_new(self);
	hx_seq_t *unsliced = HxSeq_new(NULL);
	while(HxStrIter_next(characters))
	{
		HxSeq_append(unsliced, HxStrIter_get(characters));
	}

	hx_str_t *joinat = HxStr_new("");
	hx_seq_t *sliced = HxSeq_slice(unsliced, start, stop, step);
	hx_str_t *str = HxStr_join(joinat, sliced);

	HxStr_del(joinat);
	HxStrIter_del(characters);
	HxSeq_del(unsliced);
	HxSeq_del(sliced);
	return str;
}


hx_str_t *
HxStr_from_int(int d)
{
	char *s = (char *)Hx_Utils_allocate(Hx_Utils_get_length_of_long_if_string(d) * sizeof(char));
	sprintf(s, "%d", d);
	hx_str_t *str = HxStr_new(s);
	free(s);
	return str;
}


hx_str_t *
HxStr_from_long(long d)
{
	char *s = (char *)Hx_Utils_allocate(Hx_Utils_get_length_of_long_if_string(d) * sizeof(char));
	sprintf(s, "%li", d);
	hx_str_t *str = HxStr_new(s);
	free(s);
	return str;
}


hx_str_t *
HxStr_from_float(float d, int precision)
{
	char *s = (char *)Hx_Utils_allocate(Hx_Utils_get_length_of_double_if_string(d, precision) * sizeof(char));
	sprintf(s, "%f", d);
	hx_str_t *str = HxStr_new(s);
	free(s);
	return str;
}


hx_str_t *
HxStr_from_double(double d, int precision)
{
	char *s = (char *)Hx_Utils_allocate(Hx_Utils_get_length_of_double_if_string(d, precision) * sizeof(char));
	sprintf(s, "%f", d);
	hx_str_t *str = HxStr_new(s);
	free(s);
	return str;
}


long
HxStr_to_long(hx_str_t *self, int base)
{
	char *ends_at;
	errno = 0;
	long li = strtol(self->s, &ends_at, base);
	if (errno != 0)
	{
		printf("%s: Can't convert '%s' to long.",
				HxExc_get_error_name(HxValueError),
				self->s);
		exit(HxValueError);
	}
	return li;
}


bool
HxStr_to_bool(void *self)
{
	return (bool)HxStr_len((hx_str_t *)self);
}


double
HxStr_to_double(hx_str_t *self)
{
	char *endsat;
	double dbl = strtod(self->s, &endsat);
	return dbl;
}


hx_str_iter_t *
HxStrIter_new(hx_str_t *str)
{
	hx_str_iter_t *it = (hx_str_iter_t *)Hx_Utils_allocate(sizeof(hx_str_iter_t));
	it->str = str;
	it->initiated = false;
	return it;
}


bool
HxStrIter_next(hx_str_iter_t *self)
{
	if (!self->initiated)
	{
		self->current = 0;
		self->initiated = true;
	}
	else
	{
		self->current++;
	}
	return self->current < HxStr_len(self->str);
}


void
HxStrIter_rewind(hx_str_iter_t *self)
{
	self->initiated = false;
}


hx_str_t *
HxStrIter_get(hx_str_iter_t *self)
{
	char s[2];
	strncpy(s, self->str->s + self->current, 1);
	s[1] = '\0';
	return HxStr_new(s);
}


void
HxStrIter_del(hx_str_iter_t *self)
{
	free(self);
}
