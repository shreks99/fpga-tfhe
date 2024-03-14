#include "fpga_constants.h"


//result= (X^{a}-1)*source
void torusPolynomialMulByXaiMinusOne_FPGA(TorusPolynomial_FPGA *result, int32_t a, const TorusPolynomial_FPGA *source) {    
  //    assert(a >= 0 && a < 2 * N);

  if (a < N) {
    for (int32_t i = 0; i < a; i++) { // Case where i - a < 0
      result->coefsT[i] = -source->coefsT[i - a + N] - source->coefsT[i];
    }
    for (int32_t i = a; i < N; i++) { // Case where N > i - a >= 0
      result->coefsT[i] = source->coefsT[i - a] - source->coefsT[i];
    }
  } else {
    const int32_t aa = a - N;
    for (int32_t i = 0; i < aa; i++) { // Case where i - aa < 0
      result->coefsT[i] = source->coefsT[i - aa + N] - source->coefsT[i];
    }
    for (int32_t i = aa; i < N; i++) { // Case where N > i - aa >= 0
      result->coefsT[i] = -source->coefsT[i - aa] - source->coefsT[i];
    }
  }
}

void tLweMulByXaiMinusOne_FPGA(TLweSample_FPGA *out, int32_t ai, TLweSample_FPGA* in) {
    // Assuming k is the number of polynomials in the mask, defined globally
    for (int32_t i = 0; i <= k; i++) {
        torusPolynomialMulByXaiMinusOne_FPGA(&out->a[i], ai, &in->a[i]);
        printTorusPolynomialFPGA(&out->a[i],logtLweMulByXaiMinusOneFile);
        printTorusPolynomialFPGA(&in->a[i],logtLweMulByXaiMinusOneFile);
    }
}

void torusPolynomialAddTo_FPGA(TorusPolynomial_FPGA* result, const TorusPolynomial_FPGA* poly2) {
    // Assuming the size of the polynomial (N) is globally defined or part of the FPGA constants.
    for (int32_t i = 0; i < N; ++i) {
        result->coefsT[i] += poly2->coefsT[i];
    }
}
void assignBCoef(TorusPolynomial_FPGA* result, const TorusPolynomial_FPGA* poly2) {
    // Assuming the size of the polynomial (N) is globally defined or part of the FPGA constants.
    for (int32_t i = 0; i < N; ++i) {
        result->coefsT[i] = poly2->coefsT[i];
    }
}
/** result = result + sample */
void tLweAddTo_FPGA(TLweSample_FPGA *result, const TLweSample_FPGA *sample) {
    for (int32_t i = 0; i < k; ++i)
        torusPolynomialAddTo_FPGA(&result->a[i], &sample->a[i]);
    torusPolynomialAddTo_FPGA(&result->a[k], &sample->a[k]);
    assignBCoef(&result->b, &result->a[k]);
    result->current_variance += sample->current_variance;
}

extern "C" void tfhe_MuxRotate_FFT_FPGA(TLweSample_FPGA *result, TLweSample_FPGA *accum,const TGswSampleFFT_FPGA *bki, int32_t barai) {  
  #pragma HLS INTERFACE m_axi port=result
  #pragma HLS INTERFACE m_axi port=accum
  #pragma HLS INTERFACE m_axi port=bki
  #pragma HLS INTERFACE m_axi port=barai

 logMuxRotateFile << "Before tLweMulByXaiMinusOne:\n";
   // printTLweSampleFPGA(result, logMuxRotateFile);

  // ACC = BKi*[(X^barai-1)*ACC]+ACC
  // temp = (X^barai-1)*ACC
  tLweMulByXaiMinusOne_FPGA(result, barai, accum);

   logMuxRotateFile << "After tLweMulByXaiMinusOne:\n";
   printTLweSampleFPGA(result, logMuxRotateFile);

  // temp *= BKi
  tGswFFTExternMulToTLwe_FPGA(result, bki);

    logMuxRotateFile << "After tGswFFTExternMulToTLwe:\n";
    printTLweSampleFPGA(accum, logMuxRotateFile);
  logMuxRotateFile << "Result ::\n";
   printTLweSampleFPGA(result, logMuxRotateFile);

  // ACC += temp
  tLweAddTo_FPGA(result, accum);

 logMuxRotateFile << "After tLweAddTo:\n";
   printTLweSampleFPGA(result, logMuxRotateFile);
}
