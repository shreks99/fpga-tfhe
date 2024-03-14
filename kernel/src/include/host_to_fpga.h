#ifndef HOST_TO_FPGA_H
#define HOST_TO_FPGA_H

#include "host_constants.h"
#include <iostream> 

void TorusPolynomial_Host_to_FPGA(TorusPolynomial* in, TorusPolynomial_FPGA* out) {
    for(int i = 0; i < N; i++) {
        out->coefsT[i] = in->coefsT[i];
    }
}

void LagrangeHalfCPolynomial_Host_to_FPGA(LagrangeHalfCPolynomial* in, LagrangeHalfCPolynomial_FPGA* out) {
    cplx *temp = (cplx*) in->data;
    for(int i = 0; i < N/2; i++) {
        //std::cout << "data : " << temp[i] << std::endl;  
        out->coefsC[i] = temp[i];
    }
}

void LweSample_Host_to_FPGA(LweSample* in, LweSample_FPGA* out) {
    out->b = in->b; 
    out->current_variance = in->current_variance; 
    for (int i = 0; i < n; i++) {
        out->a[i] = in->a[i];
    }
}

void LweParams_Host_to_FPGA(const LweParams* in, LweParams_FPGA* out) {
    out->n = in->n;
    out->alpha_min = in->alpha_min;
    out->alpha_max = in->alpha_max;
}

void TLweParams_Host_to_FPGA(const TLweParams* in, TLweParams_FPGA* out) {
    out->N = in->N;
    out->k = in->k; 
    out->alpha_min = out->alpha_min;
    out->alpha_max = out->alpha_max;
    LweParams_Host_to_FPGA(&(in->extracted_lweparams), &(out->extracted_lweparams));
}

void TGswParams_Host_to_FPGA(const TGswParams* in, TGswParams_FPGA* out) {
    out->l = in->l;
    out->Bgbit = in->Bgbit;
    out->Bg = in->Bg;
    out->halfBg = in->halfBg;
    out->maskMod = in->maskMod;
    TLweParams_Host_to_FPGA(in->tlwe_params, &(out->tlwe_params));
    out->kpl = in->kpl; 
    for (int i = 0; i < bk_l; i++) {
        out->h[i] = in->h[i];
    }
    out->offset = in->offset; 
}

void TLweSampleFFT_Host_to_FPGA(TLweSampleFFT* in, TLweSampleFFT_FPGA* out) {
    for (int i = 0; i < k + 1; i++) {
        LagrangeHalfCPolynomial_Host_to_FPGA(&(in->a[i]), &(out->a[i]));
    }
    LagrangeHalfCPolynomial_Host_to_FPGA(in->b, &(out->b));
    out->current_variance = in->current_variance; 
}

void TGswSampleFFT_Host_to_FPGA(const TGswSampleFFT* in, TGswSampleFFT_FPGA* out) {
    //out->k = in->k; 
    //out->l = in->l;
    for (int i = 0; i < (k + 1) * bk_l; i++) {
        TLweSampleFFT_Host_to_FPGA(&(in->all_samples[i]), &(out->all_samples[i]));
        //out->all_samples[i] = in->all_samples[i];
    }
}

void LweKeySwitchKey_Host_to_FPGA(const LweKeySwitchKey* in, LweKeySwitchKey_FPGA* out) {
    out->n = in->n;
    out->t = in->t; 
    out->basebit = in->basebit;
    out->base = in->base;
    LweParams_Host_to_FPGA(in->out_params, &(out->out_params));
    for (int i = 0; i < N * ks_length * (1 << ks_basebit); i++) {
        LweSample_Host_to_FPGA(&(in->ks0_raw[i]), &(out->ks0_raw[i]));
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < ks_length * (1 << ks_basebit); j++) {
            LweSample_Host_to_FPGA(&(in->ks1_raw[i][j]), &(out->ks1_raw[i][j]));
        }
    }
    for(int i = 0; i < N; i++) {
        for (int j = 0; j < ks_length; j++) {
            for (int k = 0; k < (1 << ks_basebit); k++) {
                LweSample_Host_to_FPGA(&(in->ks[i][j][k]), &(out->ks[i][j][k]));
            }
        }
    }
}

void LweBootstrappingKeyFFT_Host_to_FPGA(const LweBootstrappingKeyFFT* in, LweBootstrappingKeyFFT_FPGA* out) {
    LweParams_Host_to_FPGA(in->in_out_params, &(out->in_out_params));
    TGswParams_Host_to_FPGA(in->bk_params, &(out->bk_params));
    TLweParams_Host_to_FPGA(in->accum_params, &(out->accum_params));
    LweParams_Host_to_FPGA(in->extract_params, &(out->extract_params));
    for (int i = 0; i < n; i++){
        TGswSampleFFT_Host_to_FPGA(&(in->bkFFT[i]), &(out->bkFFT[i]));
    }
    LweKeySwitchKey_Host_to_FPGA(in->ks, &(out->ks));
}

#endif