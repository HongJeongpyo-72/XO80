/*
 * matrix_fixed.h
 *
 *  Created on: 2018¦~7¤ë19¤é
 *      Author: USER
 */

#ifndef MATRIX_FIXED_H_
#define MATRIX_FIXED_H_
#if defined(__cplusplus)
extern "C" {
#endif
#include "type_def.h"
#include "basic_op.h"
#if defined(__cplusplus)
}
#endif
#define FIXMATRIX_MAX_SIZE (16)

typedef struct {
	int64_t real;
	int64_t imag;
} ComplexInt64;

typedef struct {
	int rows;
	int cols;
	ComplexInt16 data[FIXMATRIX_MAX_SIZE][FIXMATRIX_MAX_SIZE];
} mcf16_t;

typedef struct {
	int rows;
	int cols;
	ComplexInt64 data[FIXMATRIX_MAX_SIZE][FIXMATRIX_MAX_SIZE];
} mcf64_t; //64*64*(8+8)

#if defined(__cplusplus)
extern "C" {
#endif
ComplexInt16 cfa16_dot(const ComplexInt16 *a, int a_stride,
		const ComplexInt16 *b, int b_stride, int n);
void mcf16_mul(mcf16_t *dest, const mcf16_t *a, const mcf16_t *b);
void mcf16_mul_bt(mcf16_t *dest, const mcf16_t *a, const mcf16_t *bt);
ComplexInt64 cfa64_dot(ComplexInt64 const *a, int a_stride,
		ComplexInt64 const *b, int b_stride, int n);
void mcf64_mul(mcf64_t *dest, mcf64_t const *a, mcf64_t const *b);
void mcf64_mul_ah(mcf64_t * __restrict dest, mcf64_t const *ah,
		mcf64_t const *b);
void mcf64_mul_at(mcf64_t *dest, mcf64_t const *at, mcf64_t const *b);
void mcf64_mul_bt(mcf64_t *dest, mcf64_t const *a, mcf64_t const *bt);
#if defined(__cplusplus)
}
#endif

#endif /* MATRIX_FIXED_H_ */
