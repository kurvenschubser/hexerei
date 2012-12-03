#include <stdio.h>


#define HxMemoryAllocationError 1
#define HxIndexError 2
#define HxZeroDivisionError 3
#define HxIOError 4
#define HxKeyError 5
#define HxNameError 6
#define HxNotImplementedError 7
#define HxValueError 8
#define HxSnafuError 9
#define HxGLError 10


char *
HxExc_get_error_name(int errorcode);
