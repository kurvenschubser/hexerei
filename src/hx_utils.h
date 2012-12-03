#ifndef HX_UTILS_H
#define HX_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "hx_exc.h"
#include "hx_seq.h"
#include "hx_glprogram.h"



#define Hx_length_of_decimal_if_string(d) ((d) == 0 ? 1 : floor(log10(abs(d))) + 1)


static const double PI = 3.14159265358979323846;


typedef struct
{
	bool button_is_pressed[3];
	int curx;
	int cury;
	int lastx;
	int lasty;
} mousestate_t;


typedef struct Vertex
{
	float Position[4];
	float Color[4];
} Vertex;


typedef struct Matrix
{
	float m[16];
} Matrix;


static const Matrix IDENTITY_MATRIX = { {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
} };


float Cotangent(float angle);


float DegreesToRadians(float degrees);


float RadiansToDegrees(float radians);


Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2);


void RotateAboutX(Matrix* m, float angle);


void RotateAboutY(Matrix* m, float angle);


void RotateAboutZ(Matrix* m, float angle);


void ScaleMatrix(Matrix* m, float x, float y, float z);


void TranslateMatrix(Matrix* m, float x, float y, float z);


Matrix CreateProjectionMatrix(
	float fovy,
	float aspect_ratio,
	float near_plane,
	float far_plane
);

void ExitOnGLError(const char* error_message);


GLuint LoadShader(const char* filename, GLenum shader_type);


int
Hx_Utils_random();


/*
 * @return Random pick from the interval [start, stop).
 */
long
Hx_Utils_random_pick_in_range(long start, long stop);


hx_seq_t *
Hx_Utils_quicksort(hx_seq_t *sortee, int (*cmp)(const void *, const void *));


int
Hx_Utils_get_length_of_long_if_string(long val);


int
Hx_Utils_get_length_of_double_if_string(double val, int precision);


void *
Hx_Utils_allocate(size_t n);

#endif
