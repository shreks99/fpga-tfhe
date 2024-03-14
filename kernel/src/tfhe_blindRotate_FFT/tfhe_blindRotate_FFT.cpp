#ifndef __SYNTHESIS__
#include <fstream>
#endif
#include "fpga_constants.h"
std::ofstream logMuxRotateFile("logfpga_tfhe_muxRotate.log", std::ios::out | std::ios::app);
std::ofstream logtGswFFTExternMulToTLweFile("logfpga_tGswFFTExternMulToTLwe.log", std::ios::out | std::ios::app);
std::ofstream logtLweMulByXaiMinusOneFile("logfpga_tLweMulByXaiMinusOne.log", std::ios::out | std::ios::app);
//std::ofstream logfpgaPeaseFile("logfpga_PeaseFFT.log", std::ios::out | std::ios::app);
void init_TLweSample_FPGA(TLweSample_FPGA* sample) {
    // Assuming k and N are globally defined constants for the size of the arrays
    for (int i = 0; i <= k; ++i) {
        for (int j = 0; j < N; ++j) {
            sample->a[i].coefsT[j] = 0;
        }
    }
    for (int i = 0; i < N; ++i) {
        sample->b.coefsT[i] = 0;
    }
    sample->current_variance = 0.0;
}

void tLweCopy_FPGA(TLweSample_FPGA *output,const TLweSample_FPGA* input) {
  
  #pragma HLS INTERFACE m_axi port=output
  #pragma HLS INTERFACE m_axi port=input

  
  for (int32_t i = 0; i <= k; ++i) {
    for (int32_t j = 0; j < N; ++j) {
      output->a[i].coefsT[j] = input->a[i].coefsT[j];
    }
  }
  for (int32_t i = 0; i < N; i++) {
    output->b.coefsT[i] = input->b.coefsT[i];
  }
  output->current_variance = input->current_variance;
}
#ifndef __SYNTHESIS__
// Placeholder for printing TorusPolynomial contents
extern void printTorusPolynomialFPGA(const TorusPolynomial_FPGA* poly, std::ofstream& outputFile) {
    outputFile << "Tours Coefficients: ";
    for (int32_t i = 0; i < N; ++i) {
        outputFile << poly->coefsT[i] << ", ";
    }
    outputFile << std::endl;
}
// Placeholder for printing TLweSample contents
extern void printTLweSampleFPGA(const TLweSample_FPGA* sample, std::ofstream& outputFile) {
    outputFile << "a (polynomials): \n";
    for (int i = 0; i <= k; ++i) {
        outputFile << "Polynomial " << i << ": ";
        printTorusPolynomialFPGA(&sample->a[i], outputFile);
    }
    outputFile << "b (right term): \n";
    printTorusPolynomialFPGA(&sample->b, outputFile);
    // outputFile << "Current variance: " << sample->current_variance << "\n";
}
#endif

extern "C" {
  void tfhe_blindRotate_FFT(
      // hls::stream<Torus32_FPGA, N>& in_testvect,
      TLweSample_FPGA* accum_FPGA,
      //hls::stream<Torus32_FPGA, n>& in_bara, 
      Torus32_FPGA* in_bara,
      TGswSampleFFT_FPGA *in_bkFFT, 
      TLweSample_FPGA* output
      ) {

    std::cout << "Inside tfhe_blindRotate_FFT" << std::endl;
    /*INPUT*/
    // #pragma HLS INTERFACE mode=axis port=&in_testvect depth=N 
    #pragma HLS INTERFACE m_axi port=accum_FPGA
    #pragma HLS INTERFACE m_axi port=&in_bara 
   // #pragma HLS INTERFACE mode=axis port=&in_bara depth=n
    #pragma HLS INTERFACE m_axi port=in_bkFFT 

    /*OUTPUT*/
    #pragma HLS INTERFACE m_axi port=output

    Torus32_FPGA temp_bara[n];
    //TLweSample_FPGA accum; 

    #ifndef __SYNTHESIS__
      std::ofstream logfpgaBlindRotate("logfpga_tfhe_blindRotate.log");
      // if (!outputFile.is_open()) {
          // std::cerr << "Unable to open file for writing.\n";
      // }
      // outputFile << "Reading from TLweSample stream: " << N <<"\n";
      // printTLweSampleFPGA(accum_FPGA,outputFile);
      // outputFile << "Bara ("<< n << ") :";
      // for (int i = 0; i < n; ++i) {
      //     outputFile << in_bara[i] << " ";
      // }
      // outputFile << "\n";
      logtGswFFTExternMulToTLweFile << "k" << k << "\n";
      logtGswFFTExternMulToTLweFile << "bk_l" << bk_l << "\n";
      logtGswFFTExternMulToTLweFile << "param_kpl" << param_kpl << "\n";
      logtGswFFTExternMulToTLweFile << "Bgbit" << bk_Bgbit << "\n";
      logtGswFFTExternMulToTLweFile << "halfBg" << param_halfBg << "\n";
      logtGswFFTExternMulToTLweFile << "maskMod" << param_maskMod << "\n";
      // logtGswFFTExternMulToTLweFile << "offset" << bk_params->offset << "\n";
    #endif

    // Temporal storage for computations, statically allocated
    TLweSample_FPGA temp;
    init_TLweSample_FPGA(&temp); // Initialize the temporary storage

    TLweSample_FPGA *temp2 = &temp;
    TLweSample_FPGA *temp3 = accum_FPGA;

     for (int32_t i = 0; i < 1; i++) {
      const int32_t barai = in_bara[i]; 
      if(barai == 0) continue;
        //tfhe_MuxRotate_FFT(muxRotateOutput, accum_FPGA, in_bkFFT[i], barai);
        if(i<10){
          logfpgaBlindRotate << "i ::" <<i << std::endl;
          printTLweSampleFPGA(temp3,logfpgaBlindRotate);
          logfpgaBlindRotate << std::endl;
          printTLweSampleFPGA(temp2,logfpgaBlindRotate);
        }
        tfhe_MuxRotate_FFT_FPGA(temp2, temp3, &in_bkFFT[i], barai);
       
        // Swap pointers; consider using double buffering techniques for FPGA
        TLweSample_FPGA *swap_temp = temp2;
        temp2 = temp3;
        temp3 = swap_temp;
     }

    if (temp3 != accum_FPGA) {
        tLweCopy_FPGA(accum_FPGA, temp3);
    }
    tLweCopy_FPGA(output,accum_FPGA);

    #ifndef __SYNTHESIS__
    // outputFile <<  "\nPrint the state of muxRotateOutput after each tfhe_MuxRotate_FFT call\n";
    // printTLweSampleFPGA(output, outputFile);
    // outputFile << "Completed tfhe_blindRotate_FFT" << std::endl;
    logfpgaBlindRotate.close();
    //logMuxRotateFile.close();
    logtGswFFTExternMulToTLweFile.close();
    logtLweMulByXaiMinusOneFile.close();
    //logfpgaPeaseFile.close();

    #endif
  }
}