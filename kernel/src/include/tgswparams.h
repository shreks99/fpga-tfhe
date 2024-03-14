#ifndef TGSWPARAMS_FPGA_H
#define TGSWPARAMS_FPGA_H

#pragma once 
#include "constants.h"
#include "tlweparams.h"

typedef struct TGswParams_FPGA {
    int32_t l; ///< decomp length
    int32_t Bgbit;///< log_2(Bg)
    int32_t Bg;///< decomposition base (must be a power of 2)
    int32_t halfBg; ///< Bg/2
    uint32_t maskMod; ///< Bg-1

    TLweParams_FPGA tlwe_params; ///< Params of each row
    int32_t kpl; ///< number of rows = (k+1)*l
    Torus32_FPGA h[bk_l]; ///< powers of Bgbit
    uint32_t offset; 
}__attribute__((packed)) TGswParams_FPGA;

#endif