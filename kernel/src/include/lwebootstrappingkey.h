#ifndef LWEBOOTSTRAPPINGKEY_FPGA_H
#define LWEBOOTSTRAPPINGKEY_FPGA_H

#include "constants.h"
#include "lweparams.h"
#include "tgswparams.h"
#include "tlweparams.h"
#include "tgswsample.h"
#include "lwekeyswitch.h"

#pragma once 

typedef struct LweBootstrappingKeyFFT_FPGA {
    LweParams_FPGA in_out_params; 
    TGswParams_FPGA bk_params;
    TLweParams_FPGA accum_params;
    LweParams_FPGA extract_params; 
    TGswSampleFFT_FPGA bkFFT[n];
    LweKeySwitchKey_FPGA ks;
} LweBootstrappingKeyFFT_FPGA;

#endif 