#ifndef LWEKEYSWITCH_FPGA_H
#define LWEKEYSWITCH_FPGA_H

#pragma once 
#include "lwesample.h"
#include "lweparams.h"

typedef struct LweKeySwitchKey_FPGA {
    int32_t n; ///< length of the input key: s'
    int32_t t; ///< decomposition length
    int32_t basebit; ///< log_2(base)
    int32_t base; ///< decomposition base: a power of 2 
    LweParams_FPGA out_params; ///< params of the output key s 
    LweSample_FPGA ks0_raw[N * ks_length * (1 << ks_basebit)]; //tableau qui contient tout les Lwe samples de taille nlbase
    LweSample_FPGA ks1_raw[N][ks_length * (1 << ks_basebit)];// de taille nl  pointe vers un tableau ks0_raw dont les cases sont espaceés de base positions
    LweSample_FPGA ks[N][ks_length][(1 << ks_basebit)]; ///< the keyswitch elements: a n.l.base matrix
    // de taille n pointe vers ks1 un tableau dont les cases sont espaceés de ell positions
} LweKeySwitchKey_FPGA;

#endif 