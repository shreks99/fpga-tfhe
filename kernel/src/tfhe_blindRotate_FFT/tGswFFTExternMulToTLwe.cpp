#include "fpga_constants.h"
// #include "header.h"
#include "header.h"
#include "hls_print.h"

// void tmpa_accum(TLweSampleFFT_FPGA *tmpa_final, TLweSampleFFT_FPGA tmpa[param_kpl]) {
//   loop_tmpa_final_1: for(int p=0; p<param_kpl; p++) {
//     loop_tmpa_final_2: for(int j=0; j<=k; j++) {
//       loop_tmpa_final_3: for(int i=0; i<N/2; i++) {
//         tmpa_final->a[j].coefsC[i] += tmpa[p].a[j].coefsC[i];
//       }
//     }
//   }
// }

extern "C" void tGswFFTExternMulToTLwe(TLweSample_FPGA* input, TGswSampleFFT_FPGA _gsw, TLweSample_FPGA *output) {
  
  TLweSample_FPGA accum;
  TGswSampleFFT_FPGA gsw;
  IntPolynomial_FPGA deca[param_kpl];
  LagrangeHalfCPolynomial_FPGA decaFFT[param_kpl];
  TLweSampleFFT_FPGA tmpa;
  //TLweSampleFFT_FPGA tmpa_final;

  accum.current_variance = input->current_variance;
  for(int i=0; i<=k; i++) {
    for(int j=0; j<N; j++) {
      accum.a[i].coefsT[j] = input->a[i].coefsT[j];
    }
  }
  for (int32_t i = 0; i < N; i++) {
    accum.b.coefsT[i] = input->b.coefsT[i];
  }

  // for(int i=0; i<(k + 1) * bk_l; i++) {
  //   for(int j=0; j<=k; j++) {
  //     for(int k=0; k<N/2; k++) {
  //       gsw.all_samples[i].a[j].coefsC[k] = _gsw.all_samples[i].a[j].coefsC[k];
  //     }
  //   }
  //   for (int32_t j = 0; j < N/2; j++) {
  //     gsw.all_samples[i].b.coefsC[j] = _gsw.all_samples[i].b.coefsC[j];
  //   }
  //   gsw.all_samples[i].current_variance = _gsw.all_samples[i].current_variance;
  // }


  // #pragma HLS array_partition variable=deca complete dim=1
  // #pragma HLS array_partition variable=decaFFT complete dim=1
  // #pragma HLS array_partition variable=tmpa complete dim=1
  // #pragma HLS array_partition variable=tmpa_final.a complete dim=1


  for (int32_t i = 0; i <= k; i++) {
    tGswTorus32PolynomialDecompH(accum.a[i], &(deca[i * bk_l]));
  }
  
  IntPolynomial_ifft(deca, decaFFT);

  //tLweFFTClear(tmpa);
  
  tLweFFTAddMulRTo(decaFFT, _gsw, &tmpa);
  //tmpa_accum(&tmpa_final, tmpa);
  tLweFromFFTConvert(&accum, &tmpa);

  output->current_variance = accum.current_variance;
  for(int i=0; i<=k; i++) {
    for(int j=0; j<N; j++) {
      output->a[i].coefsT[j] = accum.a[i].coefsT[j];
    }
  }
  for (int32_t i = 0; i < N; i++) {
    output->b.coefsT[i] = accum.b.coefsT[i];
  }
}
void decompose(IntPolynomial_FPGA *result, TorusPolynomial_FPGA *sample) {
  // Utilize parallel processing capabilities of FPGA here
  for (int32_t j = 0; j < N; ++j) {
        sample->coefsT[j] += 2164391936;
    }

    // Then, do the decomposition (in parallel)
    for (int32_t p = 0; p < bk_l; ++p) {
        const int32_t decal = (32 - (p + 1) * bk_Bgbit);
        for (int32_t j = 0; j < N; ++j) {
            uint32_t temp1 = (sample->coefsT[j] >> decal) & param_maskMod;
            result[p].coefs[j] = temp1 - param_halfBg;
        }
    }

    // Finally, remove offset to everyone
    for (int32_t j = 0; j < N; ++j) {
        sample->coefsT[j] -= 2164391936;
    }
}

void ifft(LagrangeHalfCPolynomial_FPGA &result,const IntPolynomial_FPGA &source) {
  cmplx_type inputFFT[2 * N];
  cmplx_type outputFFT[2 * N];

  for (int32_t i = 0; i < N; i++) {
    inputFFT[i].real = source.coefs[i] / 2.0;
    inputFFT[i].imag = 0.0; 
    inputFFT[N + i].real = -inputFFT[i].real;
    inputFFT[N + i].imag = 0.0; 
  }

  // logtGswFFTExternMulToTLweFile << "Before fft_transform_reverse ::\n";
  // for (int32_t i = 0; i < 2* N; i++) {
  //   logtGswFFTExternMulToTLweFile << "Coef[" << i << "]: " 
  //           << inputFFT[i].real << " + " 
  //           << inputFFT[i].imag << "i" << "   ";
  // }
  // logtGswFFTExternMulToTLweFile << std::endl;
  pease_fft(inputFFT, outputFFT);
  // logtGswFFTExternMulToTLweFile << "After fft_transform_reverse ::\n";
   
   for (int32_t i = 0; i < 2*N; i++) {
    outputFFT[i].imag *= -1.;
    // logtGswFFTExternMulToTLweFile << "Coef[" << i << "]: " 
    //         << outputFFT[i].real << " + " 
    //         << outputFFT[i].imag << "i" << "   ";
  }
    logtGswFFTExternMulToTLweFile << std::endl;

  for (int32_t i=0; i<N; i+=2) {
    result.coefsC[(int)(i/2)] = std::complex<double>(outputFFT[i + 1].real,outputFFT[i + 1].imag);
    //result.coefsC[i].imag = outputFFT[2 * i + 1].imag;
  }
}

extern void printIntPolynomialFPGA(const IntPolynomial_FPGA* poly, std::ofstream& logFile) {
  logFile << "Int Coefficients: ";
    for (int32_t i = 0; i < N; ++i) {
        logFile << poly->coefs[i] << " ";
    }
    logFile << std::endl;
}
extern void printLagrangeHalfCPolynomialCoefsCFPGA(const LagrangeHalfCPolynomial_FPGA* poly, std::ofstream& logFile) {

  logFile << "\nLagrangeHalfCPolynomial_FPGA Coefficients (coefsC):" << std::endl;
  // Loop over each coefficient and print the real and imaginary parts
  for (int32_t i = 0; i < N/2; ++i) {
      logFile << "Coef[" << i << "]: " 
              << poly->coefsC[i].real() << " + " 
              << poly->coefsC[i].imag() << "i" << "   ";
  }
}
extern void printTLweSamplefftFPGA(const TLweSampleFFT_FPGA* sample, std::ofstream& outputFile) {
  outputFile << "TLweSampleFFT k: " << k << "\n";
    // Assuming a and b are arrays of length k+1
    outputFile << "TLweSampleFFT a (polynomials): \n";
    for (int i = 0; i <= k; ++i) {
        printLagrangeHalfCPolynomialCoefsCFPGA(&sample->a[i], outputFile);
    }
    
}


void LagrangeHalfCPolynomialClear_FPGA(LagrangeHalfCPolynomial_FPGA *reps) {
    for (int32_t i = 0; i < N/2; ++i) {
        reps->coefsC[i] = 0; 
    }
}

void tLweFFTClear_FPGA(TLweSampleFFT_FPGA *result) {
    for (int32_t i = 0; i <= k; ++i) {
        LagrangeHalfCPolynomialClear_FPGA(&result->a[i]);
    }
    result->current_variance = 0.0;
}
void LagrangeHalfCPolynomialAddMul_FPGA(LagrangeHalfCPolynomial_FPGA* accum, 
                                        const LagrangeHalfCPolynomial_FPGA* a, 
                                        const LagrangeHalfCPolynomial_FPGA* b) {
    // Assuming Ns2 is the half size of the N, the number of elements in the polynomial
    for (int32_t i = 0; i < N / 2; i++) {
      // Perform termwise multiplication and add the result to the accumulator
      accum->coefsC[i] += (a->coefsC[i] * b->coefsC[i]);
    }
}
// Assuming fft_multiply_accumulate is defined to perform the actual multiplication and accumulation.
void fft_multiply_accumulate(TLweSampleFFT_FPGA *accum, const LagrangeHalfCPolynomial_FPGA &decaFFT, const TLweSampleFFT_FPGA &gsw_sample) {
  for (int32_t i = 0; i <= k; i++) {
    // Perform the termwise multiplication and add the result to the accumulator
    // printLagrangeHalfCPolynomialCoefsCFPGA(&(accum->a[i]),logtGswFFTExternMulToTLweFile);
    // printLagrangeHalfCPolynomialCoefsCFPGA(&decaFFT,logtGswFFTExternMulToTLweFile);
    // printLagrangeHalfCPolynomialCoefsCFPGA(&(gsw_sample.a[i]),logtGswFFTExternMulToTLweFile);
    // logtGswFFTExternMulToTLweFile<< "\n";
    LagrangeHalfCPolynomialAddMul_FPGA(&(accum->a[i]), &decaFFT, &(gsw_sample.a[i]));
  }
}
void fft_multiply_accumulate_all(TLweSampleFFT_FPGA *accum, const LagrangeHalfCPolynomial_FPGA *decaFFT, const TGswSampleFFT_FPGA &gsw) {
  for (int i = 0; i < param_kpl; ++i) {
        // For each sample in the array, perform FFT-based multiplication.
        fft_multiply_accumulate(accum, decaFFT[i], gsw.all_samples[i]);
    }
}

void from_fft_convert_FPGA(TLweSample_FPGA *result, const TLweSampleFFT_FPGA *source) {
    // Convert from FFT domain back to time domain
}
// FPGA version of tGswFFTExternMulToTLwe
extern "C" void tGswFFTExternMulToTLwe_FPGA(TLweSample_FPGA *accum,const TGswSampleFFT_FPGA *gsw) {
      // std::cout<< "\nEntered tGswFFTExternMulToTLwe_FPGA ::" << N;
    IntPolynomial_FPGA deca[param_kpl];
    LagrangeHalfCPolynomial_FPGA decaFFT[param_kpl];
    TLweSampleFFT_FPGA tmpa;
   // logtGswFFTExternMulToTLweFile << "Before tGswTorus32PolynomialDecompH (deca)" << std::endl;
   // printIntPolynomialFPGA(&deca[0], logtGswFFTExternMulToTLweFile);

    // Decomposition and FFT - simplified version
    for (int i = 0; i <= k; i++) {
        // Decomposition
        decompose(deca + i * bk_l, accum->a + i);
       // printIntPolynomialFPGA(deca + i * bk_l, logtGswFFTExternMulToTLweFile);
       // printTorusPolynomialFPGA(accum->a + i, logtGswFFTExternMulToTLweFile);

    }
    //logtGswFFTExternMulToTLweFile << "\nAfter tGswTorus32PolynomialDecompH (deca)" << std::endl;

    //logtGswFFTExternMulToTLweFile << "\nBefore IntPolynomial_ifft(decaFFT)" << std::endl;
    // printLagrangeHalfCPolynomialCoefsCFPGA(decaFFT,logtGswFFTExternMulToTLweFile);

    // Perform FFT on decomposed polynomials - assuming FFT function exists
    for (int p = 0; p < param_kpl; p++) {
        ifft(decaFFT[p], deca[p]);
       //printLagrangeHalfCPolynomialCoefsCFPGA(decaFFT + p,logtGswFFTExternMulToTLweFile);
       // printIntPolynomialFPGA(deca + p, logtGswFFTExternMulToTLweFile);
    }

    // logtGswFFTExternMulToTLweFile << "\nAfter IntPolynomial_ifft(decaFFT)" << std::endl;
    // printLagrangeHalfCPolynomialCoefsCFPGA(decaFFT,logtGswFFTExternMulToTLweFile);

    // Clearing tmpa and performing FFT-based multiplication
    tLweFFTClear_FPGA(&tmpa);
    fft_multiply_accumulate_all(&tmpa, decaFFT, *gsw);

   // logtGswFFTExternMulToTLweFile << "After tLweFFTAddMulRTo(tmpa)" << std::endl;
   // printTLweSamplefftFPGA(&tmpa,logtGswFFTExternMulToTLweFile);


    // Convert back from FFT domain to time domain and store in accum
    tLweFromFFTConvert(accum, &tmpa);

   logtGswFFTExternMulToTLweFile << "\nAfter tLweFromFFTConvert(accum)" << std::endl;
   printTLweSampleFPGA(accum, logtGswFFTExternMulToTLweFile);
}
