#include "fpga_constants.h"

// void tGswTorus32PolynomialDecompH_dataflow(IntPolynomial_FPGA result[bk_l], const TorusPolynomial_FPGA *sample) {
//   uint32_t buf[N];

//   //First, add offset to everyone
//   tGswTorus32PolynomialDecompH_loop_1: for (int32_t j = 0; j < N; ++j) {
//     buf[j] = (uint32_t)(sample->coefsT[j] + 2164391936);
//   }

//   //then, do the decomposition (in parallel)
//   tGswTorus32PolynomialDecompH_loop_2: for (int32_t p = 0; p < bk_l; ++p){
//     const int32_t decal = (32 - ((p + 1) * bk_Bgbit));
//     tGswTorus32PolynomialDecompH_loop_3: for (int32_t j = 0; j < N; ++j){
//       uint32_t temp1 = (buf[j] >> decal) & param_maskMod;
//       result[p].coefs[j] = temp1 - param_halfBg;
//     }
//   }
// }

// void tGswTorus32PolynomialDecompH(IntPolynomial_FPGA *result, TLweSample_FPGA *accum) {
//   #pragma HLS dataflow
//   // for(int i=0; i<=param_k; i++) {
//   tGswTorus32PolynomialDecompH_dataflow(&result[0], &accum->a[0]);
//   tGswTorus32PolynomialDecompH_dataflow(&result[3], &accum->a[1]);
//   // }
// }

extern "C" void tGswTorus32PolynomialDecompH(TorusPolynomial_FPGA input, IntPolynomial_FPGA* output) {

  uint32_t buf[N];
  for (int32_t j = 0; j < N; ++j) {
    buf[j] = (uint32_t)(input.coefsT[j] + 2164391936);
  }

  for (int32_t p = 0; p < bk_l; ++p) {
    const int32_t decal = (32 - ((p + 1) * bk_Bgbit));
    for (int32_t j = 0; j < N; ++j){
      uint32_t temp1 = (buf[j] >> decal) & param_maskMod;
      output[p].coefs[j] = temp1 - param_halfBg;
    }
  }

}

