#ifndef LWEPARAMS_FPGA_H
#define LWEPARAMS_FPGA_H

//#pragma once 
#include "constants.h"

typedef struct {
	int32_t n;
	double alpha_min;//le plus petit bruit tq sur
	double alpha_max;//le plus gd bruit qui permet le déchiffrement
} __attribute__((packed)) LweParams_FPGA;

#endif 