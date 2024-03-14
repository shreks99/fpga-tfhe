#ifndef VERIFY_H
#define VERIFICATION_H

#include "host_constants.h" 

bool verifyLwesample(LweSample_FPGA* host, LweSample_FPGA* device) {
    bool match = true; 
    std::cout << "VERIFYING LWESAMPLE..." << std::endl; 
    if (host->b != device->b) {
        std::cout << "host->b: " << host->b << " device->b " << device->b << std::endl; 
        match = false; 
    }
    for (int i = 0; i < N; i++) {
        if (host->a[i] != device->a[i]) {
            std::cout << "host->a: " << host->a[i] << " device->a " << device->a[i] << std::endl;
            match = false; 
        }
    }
    if (host->current_variance != device->current_variance) {
        std::cout << "host->current_variance: " << host->b << " device->current_variance " << device->current_variance << std::endl; 
        match = false; 
    }

    return match; 
}

bool verifyLweparams(LweParams_FPGA* host, LweParams_FPGA *device) {
    bool match = true; 
    std::cout << "VERIFYING LWEPARAMS..." << std::endl; 
    if (host->n != device->n) {
        std::cout << "host->n: " << host->n << " device->n " << device->n << std::endl; 
        match = false; 
    }
    if (host->alpha_min != device->alpha_min) {
        std::cout << "host->alpha_min: " << host->alpha_min << " device->alpha_min " << device->alpha_min << std::endl; 
        match = false; 
    }
    if (host->alpha_max != device->alpha_max) {
        std::cout << "host->alpha_max: " << host->alpha_max << " device->alpha_max " << device->alpha_max << std::endl; 
        match = false; 
    }
    return match; 
}

bool verifyTlweparams(TLweParams_FPGA* host, TLweParams_FPGA* device) {
    bool match = true; 
    std::cout << "VERIFYING TLWEPARAMS..." << std::endl; 
    if (host->N != device->N) {
        std::cout << "host->N: " << host->N  << " device->N " << device->N << std::endl; 
        match = false; 
    }

    if (host->alpha_min != device->alpha_min) {
        std::cout << "host->alpha_min: " << host->alpha_min  << " device->alpha_min " << device->alpha_min << std::endl; 
        match = false; 
    }

    if (host->alpha_max != device->alpha_max) {
        std::cout << "host->alpha_max: " << host->alpha_max  << " device->alpha_max " << device->alpha_max << std::endl; 
        match = false; 
    }

    // if (host->extracted_lweparams.n != device->extracted_lweparams.n) {
    //     std::cout << "host->extracted_lweparams.n: " << host->extracted_lweparams.n  << " device->extracted_lweparams.n " << device->extracted_lweparams.n << std::endl; 
    //     match = false; 
    // }

    // if (host->extracted_lweparams.alpha_min != device->extracted_lweparams.alpha_min) {
    //     std::cout << "host->extracted_lweparams.alpha_min: " << host->extracted_lweparams.alpha_min  << " device->extracted_lweparams.alpha_min " << device->extracted_lweparams.alpha_min << std::endl; 
    //     match = false; 
    // }

    // if (host->extracted_lweparams.alpha_max != device->extracted_lweparams.alpha_max) {
    //     std::cout << "host->extracted_lweparams.alpha_max: " << host->extracted_lweparams.alpha_max  << " device->extracted_lweparams.alpha_max " << device->extracted_lweparams.alpha_max << std::endl; 
    //     match = false; 
    // }
    match = verifyLweparams(&(host->extracted_lweparams), &(device->extracted_lweparams));

    return match; 
}

bool verifyTgswparams(TGswParams_FPGA* host, TGswParams_FPGA* device) {
    bool match = true; 

    if (host->l != device->l) {
        std::cout << "host->l: " << host->l  << " device->l " << device->l << std::endl; 
        match = false; 
    }

    if (host->Bgbit != device->Bgbit) {
        std::cout << "host->Bgbit: " << host->Bgbit  << " device->Bgbit " << device->Bgbit << std::endl; 
        match = false; 
    }

    if (host->Bg != device->Bg) {
        std::cout << "host->Bg: " << host->Bg  << " device->Bg " << device->Bg << std::endl; 
        match = false; 
    }

    if (host->halfBg != device->halfBg) {
        std::cout << "host->halfBg: " << host->halfBg  << " device->halfBg " << device->halfBg << std::endl; 
        match = false; 
    }

    if (host->maskMod != device->maskMod) {
        std::cout << "host->maskMod: " << host->maskMod  << " device->maskMod " << device->maskMod << std::endl; 
        match = false; 
    }

    match = verifyTlweparams(&(host->tlwe_params), &(device->tlwe_params));
    
    if (host->kpl != device->kpl) {
        std::cout << "host->kpl: " << host->kpl  << " device->kpl " << device->kpl << std::endl; 
        match = false; 
    }

    for (int i = 0; i < bk_l; i++) {
        if (host->h[i] != device->h[i]) {
            std::cout << "host->h: " << host->h[i] << " device->h " << device->h[i] << std::endl;
            match = false; 
        }
    }

    if (host->offset != device->offset) {
        std::cout << "host->offset: " << host->offset  << " device->offset " << device->offset << std::endl; 
        match = false; 
    }
    return match;
}

#endif