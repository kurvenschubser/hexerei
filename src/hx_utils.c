#include <stdlib.h>
#include <time.h>

#include "hx_utils.h"


bool HX_UTILS_IS_RAND_INITIALIZED = false;


float Cotangent(float angle)
{
	return (float)(1.0 / tan(angle));
}


float DegreesToRadians(float degrees)
{
	return degrees * (float)(PI / 180);
}

float RadiansToDegrees(float radians)
{
	return radians * (float)(180 / PI);
}

Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2)
{
	Matrix out = IDENTITY_MATRIX;
	unsigned int row, column, row_offset;

	for (row = 0, row_offset = row * 4; row < 4; ++row, row_offset = row * 4)
		for (column = 0; column < 4; ++column)
			out.m[row_offset + column] =
				(m1->m[row_offset + 0] * m2->m[column + 0]) +
				(m1->m[row_offset + 1] * m2->m[column + 4]) +
				(m1->m[row_offset + 2] * m2->m[column + 8]) +
				(m1->m[row_offset + 3] * m2->m[column + 12]);

	return out;
}

void ScaleMatrix(Matrix* m, float x, float y, float z)
{
	Matrix scale = IDENTITY_MATRIX;

	scale.m[0] = x;
	scale.m[5] = y;
	scale.m[10] = z;

	memcpy(m->m, MultiplyMatrices(m, &scale).m, sizeof(m->m));
}

void TranslateMatrix(Matrix* m, float x, float y, float z)
{
	Matrix translation = IDENTITY_MATRIX;

	translation.m[12] = x;
	translation.m[13] = y;
	translation.m[14] = z;

	memcpy(m->m, MultiplyMatrices(m, &translation).m, sizeof(m->m));
}


/*
 * @synopsis: In-place rotation of a matrix on x-axis by the
 * specified degree.
 * @param *m*: Pointer to the matrix to be rotated.
 * @param *angle*: degree of rotation in radians.
 */
void RotateAboutX(Matrix* m, float angle)
{
	Matrix rotation = IDENTITY_MATRIX;
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);

	rotation.m[5] = cosine;
	rotation.m[6] = -sine;
	rotation.m[9] = sine;
	rotation.m[10] = cosine;

	memcpy(m->m, MultiplyMatrices(m, &rotation).m, sizeof(m->m));
}



/*
 * @synopsis: In-place rotation of a matrix on y-axis by the
 * specified degree.
 * @param *m*: Pointer to the matrix to be rotated.
 * @param *angle*: degree of rotation in radians.
 */
void RotateAboutY(Matrix* m, float angle)
{
	Matrix rotation = IDENTITY_MATRIX;
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);

	rotation.m[0] = cosine;
	rotation.m[8] = sine;
	rotation.m[2] = -sine;
	rotation.m[10] = cosine;

	memcpy(m->m, MultiplyMatrices(m, &rotation).m, sizeof(m->m));
}


/*
 * @synopsis: In-place rotation of a matrix on z-axis by the
 * specified degree.
 * @param *m*: Pointer to the matrix to be rotated.
 * @param *angle*: degree of rotation in radians.
 */
void RotateAboutZ(Matrix* m, float angle)
{
	Matrix rotation = IDENTITY_MATRIX;
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);

	rotation.m[0] = cosine;
	rotation.m[1] = -sine;
	rotation.m[4] = sine;
	rotation.m[5] = cosine;

	memcpy(m->m, MultiplyMatrices(m, &rotation).m, sizeof(m->m));
}

Matrix CreateProjectionMatrix(
	float fovy,
	float aspect_ratio,
	float near_plane,
	float far_plane
)
{
	Matrix out = { { 0 } };

	const float
		y_scale = Cotangent(DegreesToRadians(fovy / 2)),
		x_scale = y_scale / aspect_ratio,
		frustum_length = far_plane - near_plane;

	out.m[0] = x_scale;
	out.m[5] = y_scale;
	out.m[10] = -((far_plane + near_plane) / frustum_length);
	out.m[11] = -1;
	out.m[14] = -((2 * near_plane * far_plane) / frustum_length);

	return out;
}

void ExitOnGLError(const char* error_message)
{
	const GLenum ErrorValue = glGetError();

	if (ErrorValue != GL_NO_ERROR)
	{
		const char* APPEND_DETAIL_STRING = ": %s\n";
		const size_t APPEND_LENGTH = strlen(APPEND_DETAIL_STRING) + 1;
		const size_t message_length = strlen(error_message);
		char* display_message = (char *)Hx_Utils_allocate(message_length + APPEND_LENGTH);

		memcpy(display_message, error_message, message_length);
		memcpy(&display_message[message_length], APPEND_DETAIL_STRING, APPEND_LENGTH);

		fprintf(stderr, display_message, gluErrorString(ErrorValue));

		free(display_message);
		exit(EXIT_FAILURE);
	}
}


void
Hx_Utils_random_init()
{
	if (!HX_UTILS_IS_RAND_INITIALIZED)
	{
		srand(time(NULL));
		HX_UTILS_IS_RAND_INITIALIZED = true;
	}
}


int
Hx_Utils_random()
{
	Hx_Utils_random_init();
	return rand();
}


/*
 * @return Random pick from the interval [start, stop).
 */
long
Hx_Utils_random_pick_in_range(long start, long stop)
{
	if (stop - start == 0)
	{
		return 0;
	}
	return start + (long)Hx_Utils_random() % (stop - start);
}


hx_seq_t *
Hx_Utils_quicksort(hx_seq_t *sortee, int (*cmp)(const void *, const void *))
{
	if (HxSeq_len(sortee) == 0)
	{
		return HxSeq_new(NULL);
	}

	void *pivot = HxSeq_pop(sortee, Hx_Utils_random_pick_in_range(0, HxSeq_len(sortee) - 1));
	hx_seq_t *combined = HxSeq_new(NULL);
	hx_seq_t *lesser = HxSeq_new(NULL);
	hx_seq_t *greater = HxSeq_new(NULL);

	hx_seq_iter_t *it = HxSeqIter_new(sortee);

	void *obj;
	while(HxSeqIter_next(it))
	{
		obj = HxSeqIter_get(it);
		if (cmp(obj, pivot) < 0)
		{
			HxSeq_append(lesser, obj);
		}
		else
		{
			HxSeq_append(greater, obj);
		}
	}

	hx_seq_t *lesser_sorted = Hx_Utils_quicksort(lesser, cmp);
	hx_seq_t *greater_sorted = Hx_Utils_quicksort(greater, cmp);

	/* merge into one sequence */
	HxSeq_extend(combined, lesser_sorted);
	HxSeq_append(combined, pivot);
	HxSeq_extend(combined, greater_sorted);

	/* clean up */
	HxSeq_del(lesser);
	HxSeq_del(greater);
	HxSeq_del(lesser_sorted);
	HxSeq_del(greater_sorted);
	HxSeqIter_del(it);

	return combined;
}


int
Hx_Utils_get_length_of_long_if_string(long val)
{
	/* (val < 0 ? 1 : 0) : add one for minus sign */
	return (val == 0 ? 1 : ((int)(log10(fabs(val))+1) + (val < 0 ? 1 : 0)));
}


int
Hx_Utils_get_length_of_double_if_string(double val, int precision)
{
	return Hx_Utils_get_length_of_long_if_string((long)val + precision + 1);
}


void *
Hx_Utils_allocate(size_t n)
{
	void *s = malloc(n);
	if (s == NULL)
	{
		printf("%s", HxExc_get_error_name(HxMemoryAllocationError));
		exit(HxMemoryAllocationError);
	}
	return s;
}
