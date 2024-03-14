#ifndef HOST_CONSTANTS_H
#define HOST_CONSTANTS_H

#pragma once 

/* C++ */
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <sys/time.h>
#include <iostream>

/* FPGA TFHE*/
#include "constants.h"
#include "lweparams.h"
#include "lwesample.h"
#include "polynomials.h"
#include "tlweparams.h"
#include "tlwesample.h"
#include "tgswsample.h"
#include "tgswparams.h"
#include "lwebootstrappingkey.h"
#include "lwekeyswitch.h"

/*C++ TFHE*/
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"
#include "tfhe/tfhe_core.h"
#include "tfhe/numeric_functions.h"
#include "tfhe/lweparams.h"
#include "tfhe/lwekey.h"
#include "tfhe/lwesamples.h"
#include "tfhe/lwekeyswitch.h"
#include "tfhe/lwe-functions.h"
#include "tfhe/lwebootstrappingkey.h"


int32_t modSwitchFromTorus32(Torus32_FPGA phase, int32_t Msize){
    uint64_t interv = ((UINT64_C(1)<<63)/Msize)*2; // width of each intervall
    uint64_t half_interval = interv/2; // begin of the first intervall
    uint64_t phase64 = (uint64_t(phase)<<32) + half_interval;
    //floor to the nearest multiples of interv
    return phase64/interv;
}

#endif