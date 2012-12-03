#include <unistd.h>
#include <sys/stat.h>

#include "hx_path.h"



hx_str_t *
Hx_Path_get_path_sep()
{
	return HxStr_new(HX_PATH_SEP);
}


bool
Hx_Path_exists(hx_str_t *path)
{
	struct stat fattr;
	return stat(path->s, &fattr) == 0;
}


hx_seq_t *
Hx_Path_split(hx_str_t *path)
{
	return HxStr_split(path, Hx_Path_get_path_sep());
}


hx_str_t *
Hx_Path_join(hx_str_t* path, hx_str_t *tobejoined)
{
	hx_str_t *sep = Hx_Path_get_path_sep();
	hx_str_t *s;
	if (HxStr_endswith(path, sep))
	{
		s = HxStr_add(path, tobejoined);
	}
	else
	{
		hx_seq_t *seq = HxSeq_new(path, tobejoined, NULL);
		s = HxStr_join(sep, seq);
		HxSeq_del(seq);
	}
	HxStr_del(sep);
	return s;
}
