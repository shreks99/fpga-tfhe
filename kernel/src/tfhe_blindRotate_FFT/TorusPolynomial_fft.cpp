#include "fpga_constants.h"
#include "header.h"

extern "C" void TorusPolynomial_fft(LagrangeHalfCPolynomial_FPGA p, TorusPolynomial_FPGA *results) {

  cmplx_type input[2*N];
  cmplx_type output[2*N];

  const double _2p32 = double(INT64_C(1)<<32);
  const double _1sN = double(1)/double(N);

  for (int32_t i=0; i<N; i++) input[2*i].real=0;
  for (int32_t i=0; i<N; i++) input[2*i].imag=0;
  for (int32_t i=0; i<N/2; i++) input[2*i+1].real=std::real(p.coefsC[i]);
  for (int32_t i=0; i<N/2; i++) input[2*i+1].imag=std::imag(p.coefsC[i]);
  for (int32_t i=0; i<N/2; i++) input[2*N-1-2*i].real=std::real(p.coefsC[i]);
  for (int32_t i=0; i<N/2; i++) input[2*N-1-2*i].imag=-std::imag(p.coefsC[i]);

// logtGswFFTExternMulToTLweFile << "Before execute_direct_torus32 ::\n";
// printLagrangeHalfCPolynomialCoefsCFPGA(&p,logtGswFFTExternMulToTLweFile);
// logtGswFFTExternMulToTLweFile << std::endl;
//   for (int32_t i = 0; i <  N; i++) {
//     logtGswFFTExternMulToTLweFile << "Coef[" << i << "]: " 
//             << input[i].real << " + " 
//             << input[i].imag << "i" << "   ";
//   }
//   logtGswFFTExternMulToTLweFile << std::endl;

  pease_fft(input, output);

  // logtGswFFTExternMulToTLweFile << "After execute_direct_torus32 ::\n";
  // for (int32_t i = 0; i <  N; i++) {
  //   logtGswFFTExternMulToTLweFile << "Coef[" << i << "]: " 
  //           << output[i].real << " + " 
  //           << output[i].imag << "i" << "   ";
  // }
  // logtGswFFTExternMulToTLweFile << std::endl;

  for (int32_t i=0; i<N; i++) {
    results->coefsT[i] = (Torus32_FPGA) (int64_t(output[i].real * _1sN * _2p32));
  }
  // printTorusPolynomialFPGA(results,logtGswFFTExternMulToTLweFile);
}
