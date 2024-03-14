#ifndef POLYNOMIAL_FPGA_H
#define POLYNOMIAL_FPGA_H

#pragma once 
#include "constants.h"

typedef struct IntPolynomial_FPGA {
    int32_t coefs[N];
} IntPolynomial_FPGA;

typedef struct TorusPolynomial_FPGA {
    Torus32_FPGA coefsT[N];
} TorusPolynomial_FPGA;

typedef struct LagrangeHalfCPolynomial_FPGA {
    cplx coefsC[N/2];
} __attribute__((packed)) LagrangeHalfCPolynomial_FPGA;

#endif 