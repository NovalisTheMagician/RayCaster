#ifndef _MATRIX_H
#define _MATRIX_H

#include "Vector.h"

typedef struct
{
	vec2_t r1;
	vec2_t r2;
} mat2_t;

typedef struct
{
	vec3_t r1;
	vec3_t r2;
	vec3_t r3;
} mat3_t;

mat2_t mat2_identity();
mat2_t mat2_createRotation(float a);

mat2_t mat2_add(const mat2_t *m1, const mat2_t *m2);
mat2_t mat2_sub(const mat2_t *m1, const mat2_t *m2);
mat2_t mat2_mul(const mat2_t *m1, const mat2_t *m2);

vec2_t mat2_transformVec(const mat2_t *m, const vec2_t *v);

#endif
