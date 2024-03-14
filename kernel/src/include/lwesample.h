#ifndef LWESAMPLES_FPGA_H
#define LWESAMPLES_FPGA_H

//#pragma once 

#include "constants.h"

/*

	A secret key for LWE ciphertext is n bits denoted as s = (s1, s2, ... , sn) where {0, 1}. Under the secret key 
	s, a plaintext integer m is Zq is encrypted into a LWE ciphertext where LWE(m) = c = (a1, ... , an, b) where 
	a1, ... , an are n random intergers sampled form Zq and b = a * s + m + e. 

*/
typedef struct LweSample_FPGA {
	Torus32_FPGA a[n]; //-- the n coefs of the mask
    Torus32_FPGA b;  //
   	double current_variance; 
} LweSample_FPGA;

#endif 