#ifndef HX_PATH_H
#define HX_PATH_H

#include <stdlib.h>
#include <stdbool.h>

#include "hx_seq.h"
#include "hx_str.h"


static const char *HX_PATH_SEP = "/";


hx_str_t *
Hx_Path_get_path_sep();


bool
Hx_Path_exists(hx_str_t *path);


hx_seq_t *
Hx_Path_split(hx_str_t *path);


hx_str_t *
Hx_Path_join(hx_str_t* path, hx_str_t *tobejoined);

#endif /* HX_PATH_H */
