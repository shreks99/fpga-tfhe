#include "fpga_constants.h"


// void LagrangeHalfCPolynomialAddMul(LagrangeHalfCPolynomial_FPGA *accum, LagrangeHalfCPolynomial_FPGA *a, LagrangeHalfCPolynomial_FPGA *b) {
//   for (int32_t i=0; i<N/2; i++) {
//     accum->coefsC[i] = a->coefsC[i] * b->coefsC[i];
//   }
// }

// void tLweFFTAddMulRTo_dataflow(TLweSampleFFT_FPGA *result, LagrangeHalfCPolynomial_FPGA *p, TLweSampleFFT_FPGA *sample) {
//   for (int32_t i = 0; i <= k; i++) {
//     LagrangeHalfCPolynomialAddMul(&result->a[i], p, &sample->a[i]);
//   }
// }

extern "C" void tLweFFTAddMulRTo(LagrangeHalfCPolynomial_FPGA *a, TGswSampleFFT_FPGA gsw, TLweSampleFFT_FPGA *result) {
  for(int i=0; i<param_kpl; i++) {
    for (int32_t j = 0; j <= k; j++) {
        for (int32_t p = 0; p < N/2; p++) {
          result->a[j].coefsC[p] = a[i].coefsC[p] * gsw.all_samples[i].a[j].coefsC[p];
        }
      
      //LagrangeHalfCPolynomialAddMul(&result->a[i], p, &sample->a[i]);
    }
    
    
    //tLweFFTAddMulRTo_dataflow(&result[i], &p[i], &gsw->all_samples[i]);
  }
}
