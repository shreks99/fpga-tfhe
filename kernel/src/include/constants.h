#ifndef CONSTANTS_H
#define CONSTANTS_H

#pragma once 

#include <complex> 

/*Defines the contants for the different levels of encryption 80 vs 128*/

#define ENCRYPT 1 //1: 128 0:80 as of now only 128 is being developed

#ifdef ENCRYPT
    // constexpr static int64_t param_n = 630;
    // constexpr static int64_t param_N = 1024;
    // constexpr static int64_t param_2N = 2 * param_N;
    // constexpr static int64_t param_Ns2 = param_N / 2;
    // constexpr static int32_t param_k = 1;
    // constexpr static int32_t param_l = 3;
    // constexpr static int32_t param_kpl = (param_k + 1) * param_l;
    // constexpr static int32_t param_Bgbit = 7;
    // constexpr static int32_t param_Bg = 1 << param_Bgbit;
    // constexpr static int32_t param_maskMod = param_Bg - 1;
    // constexpr static int32_t param_halfBg = param_Bg / 2;
    // constexpr static uint32_t param_offset = 2164391936; // tgsw.cpp - TGswParams::TGswParams()
    
    const int32_t N = 1024;
    static const int32_t k = 1;
    const int32_t n = 630;
    static const int32_t bk_l = 3;
    static const int32_t bk_Bgbit = 7;
    static const int32_t ks_basebit = 2;
    static const int32_t ks_length = 8;
    static const double ks_stdev = pow(2.,-15); //standard deviation
    static const double bk_stdev = pow(2.,-25);; //standard deviation
    static const double max_stdev = 0.012467; //max standard deviation for a 1/4 msg space

    const int32_t param_Bg = 1 << bk_Bgbit;
    const int32_t param_maskMod = param_Bg - 1;
    const int32_t param_halfBg = param_Bg / 2;
    const int32_t param_kpl = (k + 1) * bk_l;

#else 
    static const int32_t N = 1024;
    static const int32_t k = 1;
    static const int32_t n = 500;
    static const int32_t bk_l = 2;
    static const int32_t bk_Bgbit = 10;
    static const int32_t ks_basebit = 2;
    static const int32_t ks_length = 8;
    static const double ks_stdev = 2.44e-5; //standard deviation
    static const double bk_stdev = 7.18e-9; //standard deviation
    static const double max_stdev = 0.012467; //max standard deviation for a 1/4 msg space
#endif

typedef int32_t Torus32_FPGA; //avant uint32_t

typedef struct ARRAY {
    int32_t coefs[N];
} ARRAY;

typedef std::complex<double> cplx;

#endif
