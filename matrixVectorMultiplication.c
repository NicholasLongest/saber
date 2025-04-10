#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "poly.h"
#include "matrixVectorMultiplication.h"

// Polynomial multiplication (modulo q)
void poly_mult(uint16_t *a, uint16_t *b, uint16_t *res, size_t q, size_t n) {
    
    uint32_t temp[2 * n]; // Temporary array for intermediate results
    int32_t i,j;

    for (i = 0; i < 2 * n; i++) {
        temp[i] = 0;
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            temp[i + j] += (uint32_t)a[i] * b[j];
            temp[i + j] = temp[i+j] & (q-1); // Reduce modulo SABER_Q during multiplication
        }
    }
    // Reduce result back to n coefficients modulo q
    for (i = n; i < 2 * n; i++) {
        res[i - n] = (temp[i - n] - temp[i]) & (q-1);
    }

}

// Code assumes res[SABER_K][SABER_N] is set to all zeros.
void MatrixVectorMultiply(polyvec *a, uint16_t skpv[SABER_K][SABER_N], uint16_t res[SABER_K][SABER_N], uint16_t mod, int16_t transpose){

	uint16_t acc[SABER_N]; 
	int32_t i,j,k;

	if(transpose==1){
		for(i=0;i<SABER_K;i++){
			for(j=0;j<SABER_K;j++){
				poly_mult((uint16_t *)&a[j].vec[i], skpv[j], acc, SABER_Q, SABER_N);			

				for(k=0;k<SABER_N;k++){
					res[i][k]=res[i][k]+acc[k];
					res[i][k]=(res[i][k]&mod); //reduction mod p
					acc[k]=0; //clear the accumulator
				}
			
			}
		}
	}
	else{

		for(i=0;i<SABER_K;i++){
			for(j=0;j<SABER_K;j++){
				poly_mult((uint16_t *)&a[i].vec[j], skpv[j], acc, SABER_Q, SABER_N);			
				for(k=0;k<SABER_N;k++){
					res[i][k]=res[i][k]+acc[k];
					res[i][k]=res[i][k]&mod; //reduction
					acc[k]=0; //clear the accumulator
				}
			
			}
		}
	}

}

