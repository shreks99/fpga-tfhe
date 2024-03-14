#include "header.h"
#include <iostream>

extern "C" {
    void spiral_FFT(
    double *a, 
    double *b
    ) {

    #pragma HLS INTERFACE m_axi port = a bundle = gmem0
    #pragma HLS INTERFACE m_axi port = b bundle = gmem1

        /*Insert Code Here*/
        std::cout << "Start of krnl" << std::endl;  
        cmplx_type in[2048], out[2048];

        std::cout << "Init data" << std::endl;
        for (int i = 0; i < 2048; i++) {
            in[i].real = a[i];
            in[i].imag = 0.0;
        }
        std::cout << "Start FFT" << std::endl;
        pease_fft(in, out); 
        std::cout << "Fin FFT" << std::endl;
        std::cout << "Output data" << std::endl;
        for (int i = 0; i < 2048; i++) {
            b[i] = out[i].real;
          //  b_img[i] = out[i].imag;
            std::cout<< out[i].real<< "  ";
        }
    }
}
