#include "hx_str.h"


HxStr *
HxStr_new(const char *s)
{
	HxStr *hxstr = malloc(sizeof(HxStr));
	if (hxstr == NULL)
	{
		printf("MemoryAllocationError");
		return NULL;
	}

	char *tmp = malloc((strlen(s) + 1) * sizeof(char));
	if (tmp == NULL)
	{
		printf("MemoryAllocationError");
		return NULL;
	}

	strcpy(tmp, s);
	hxstr->s = tmp;
	hxstr->length = strlen(s);

	return hxstr;
}


void
HxStr_del(HxStr *self)
{
	free(self->s);
	free(self);
}


long
HxStr_len(HxStr *self)
{
	return self->length;
}


HxStr *
HxStr_join(HxStr *self, HxSeq *seq)
{
	if (HxSeq_len(seq) == 0)
	{
		return HxStr_new("");
	}

	HxSeqIter *it = HxSeqIter_new(seq);
	HxStr *hxstr;
	long len = 0;
	long n = 0;
	while(HxSeqIter_next(it))
	{
		hxstr = (HxStr *)(HxSeqIter_get(it));
		len += HxStr_len(hxstr);
		n++;
	}

	char *s = malloc((len + 1 + (n - 1) * HxStr_len(self)) * sizeof(char));
	if (s == NULL)
	{
		/* HxExc_set("MemoryAllocationError", s) */
		printf("MemoryAllocationError");
	}

	HxSeqIter_rewind(it);
	HxSeqIter_next(it);

	hxstr = (HxStr *)(HxSeqIter_get(it));
	strcpy(s, hxstr->s);

	while(HxSeqIter_next(it))
	{
		hxstr = (HxStr *)(HxSeqIter_get(it));
		strcat(s, self->s);
		strcat(s, hxstr->s);
	}
	HxSeqIter_del(it);

	return HxStr_new(s);
}


HxStr *
HxStr_add(HxStr *self, HxStr *other)
{
	return HxStr_join(HxStr_new(""), HxSeq_new(self, other, NULL));
}

