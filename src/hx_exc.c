#include "hx_exc.h"


char *
HxExc_get_error_name(int errorcode)
{
	char *s = NULL;
	switch(errorcode)
	{
		case HxMemoryAllocationError:
			s = "MemoryAllocationError";
			break;
		case HxIndexError:
			s = "IndexError";
			break;
		case HxZeroDivisionError:
			s = "ZeroDivisionError";
			break;
		case HxIOError:
			s = "IOError";
			break;
		case HxKeyError:
			s = "KeyError";
			break;
		case HxNameError:
			s = "NameError";
			break;
		case HxNotImplementedError:
			s = "NotImplementedError";
			break;
		case HxValueError:
			s = "ValueError";
			break;
		case HxSnafuError:
			s = "SnafuError";
			break;
		case HxGLError:
			s = "GLError";
			break;
		default:
			sprintf(s, "(no error name for code %d)", errorcode);
			break;
	}
	return s;
}
