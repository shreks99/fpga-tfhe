#include "fpga_constants.h"


void LagrangeHalfCPolynomialClear(LagrangeHalfCPolynomial_FPGA *reps) {
  LagrangeHalfCPolynomialClear_loop_1: for (int32_t i=0; i<N/2; i++) {
    reps->coefsC[i] = cplx(0, 0);
  }
}

void tLweFFTClear_dataflow(TLweSampleFFT_FPGA *result) {
  tLweFFTClear_loop_1: for (int32_t i = 0; i <= k; ++i) {
    LagrangeHalfCPolynomialClear(&result->a[i]);
  }
  result->current_variance = 0.0;
}

extern "C" void tLweFFTClear(TLweSampleFFT_FPGA *result) {
  #pragma HLS dataflow
  for(int p=0; p<param_kpl; p++) {
  tLweFFTClear_dataflow(&result[p]);
  //tLweFFTClear_dataflow(&result[1]);
  //tLweFFTClear_dataflow(&result[2]);
  //tLweFFTClear_dataflow(&result[3]);
  //tLweFFTClear_dataflow(&result[4]);
  //tLweFFTClear_dataflow(&result[5]);
  }
}
