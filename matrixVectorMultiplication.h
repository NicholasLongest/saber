#ifndef MatrixVectorM_H
#define MatrixVectorM_H

#include "poly.h"
#include "SABER_params.h"

void poly_mult(uint16_t *a, uint16_t *b, uint16_t *res, size_t q, size_t n);
void MatrixVectorMultiply(polyvec *a, uint16_t skpv[SABER_K][SABER_N], uint16_t res[SABER_K][SABER_N], uint16_t mod, int16_t transpose);

#endif
