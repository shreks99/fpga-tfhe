#include "fpga_constants.h"


void TorusPolynomial_fft_dataflow(TorusPolynomial_FPGA *tp, LagrangeHalfCPolynomial_FPGA *lp) {
  //#pragma HLS dataflow
  // logtGswFFTExternMulToTLweFile << "\n Before TorusPolynomial_fft(accum)" << std::endl;
  
  for(int i=0; i<=k; i++) {
    // printLagrangeHalfCPolynomialCoefsCFPGA(&lp[i], logtGswFFTExternMulToTLweFile);
    
    TorusPolynomial_fft(lp[i], &(tp[i]));
    
    // logtGswFFTExternMulToTLweFile << std::endl;
    // printTorusPolynomialFPGA(&tp[i], logtGswFFTExternMulToTLweFile);
    }

    // logtGswFFTExternMulToTLweFile << "\n After TorusPolynomial_fft(accum)" << std::endl;
  //  printTorusPolynomialFPGA(tp, logtGswFFTExternMulToTLweFile);
}

extern "C" void tLweFromFFTConvert(TLweSample_FPGA *result, TLweSampleFFT_FPGA *source) {
  TorusPolynomial_fft_dataflow(result->a, source->a);
  result->current_variance = source->current_variance;
}
