#ifndef TGSWSAMPLE_FPGA_H
#define TGSWSAMPLE_FPGA_H

#pragma once 
#include "constants.h"
#include "tlwesample.h"

typedef struct TGswSampleFFT_FPGA {
    TLweSampleFFT_FPGA all_samples[(k+1) * bk_l]; ///< TLweSample* all_sample; (k+1)l TLwe Sample
    // TODO: Reimplement `sample` when needed
    //TLweSampleFFT_FPGA sample; ///< accès optionnel aux différents blocs de taille l. (optional access to the various blocks of size l.)
    double current_variance;
    //int32_t k;
    //int32_t l;

} __attribute__((packed)) TGswSampleFFT_FPGA;

#endif 