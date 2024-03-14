#include "fpga_constants.h"
// #include "header.h"
#include "header.h"

void IntPolynomial_ifft_dataflow(IntPolynomial_FPGA input, LagrangeHalfCPolynomial_FPGA *output) {
  cmplx_type inputFFT[2 * N];
  cmplx_type outputFFT[N + 1];


  for (int32_t i=0; i<N; i++) {
    inputFFT[i].real = input.coefs[i]/2.;
    inputFFT[N+i].real = -inputFFT[i].real;
  }
  for (int32_t i = 0; i < 2 * N; i++) inputFFT[i].imag = 0.0; 

  pease_fft(inputFFT, outputFFT);

  for (int i = 0; i < N/2; i++) {
    output->coefsC[i] = outputFFT[2*i+1].real;
    output->coefsC[i] = outputFFT[2*i+1].imag; 
  }

}

extern "C" void IntPolynomial_ifft(IntPolynomial_FPGA *input, LagrangeHalfCPolynomial_FPGA *output) {
  for(int i=0; i<param_kpl; i++) {
    IntPolynomial_ifft_dataflow(input[i], &(output[i]));
  }
}
