#ifndef TLWESAMPLE_FPGA_H
#define TLWESAMPLE_FPGA_H

#pragma once 
#include "constants.h"
#include "polynomials.h"

typedef struct TLweSampleFFT_FPGA {
    LagrangeHalfCPolynomial_FPGA a[k + 1]; ///< array of length k+1: mask + right term
    LagrangeHalfCPolynomial_FPGA b; 
    // TODO: Reimplement `b` once needed...
    // LagrangeHalfCPolynomial_Collapsed b; ///< alias of a[k] to get the right term
    double current_variance; ///< avg variance of the sample
} __attribute__((packed)) TLweSampleFFT_FPGA;

typedef struct TLweSample_FPGA {
    TorusPolynomial_FPGA a[k + 1]; ///< array of length k+1: mask + right term
    TorusPolynomial_FPGA b; 
    // TODO: Reimplement `b` once needed...
    // TorusPolynomial *b; ///< alias of a[k] to get the right term
    double current_variance; ///< avg variance of the sample
} __attribute__((packed)) TLweSample_FPGA;

#endif