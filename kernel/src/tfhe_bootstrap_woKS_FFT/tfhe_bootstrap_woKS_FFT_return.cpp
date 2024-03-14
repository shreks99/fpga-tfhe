#include <fpga_constants.h>

/*

    Splits the x input into the a and b, and switches them from Torus32. Copies the mu value across a vector.

    Config File Specs:

        in_bk       <- tfhe_bootstrap_woKS_FFT 
        in_mu       <- tfhe_bootstrap_woKS_FFT 
        in_x        <- tfhe_bootstrap_woKS_FFT 

*/

extern "C" void tfhe_bootstrap_woKS_FFT_return(
    hls::stream<Torus32_FPGA, N>& in_testvect, 
    hls::stream<Torus32_FPGA, n>& in_bara, 
    hls::stream<Torus32_FPGA>& in_barb,
    Torus32_FPGA *out_testvect,
    Torus32_FPGA *out_bara,
    Torus32_FPGA *out_barb) {

    /*INPUT*/
    #pragma HLS INTERFACE mode=axis port=&in_testvect depth=N
    #pragma HLS INTERFACE mode=axis port=&in_bara depth=n
    #pragma HLS INTERFACE mode=axis port=&in_barb
    //#pragma HLS INTERFACE mode=axis port=&in_x_current_variance

    /*OUTPUT*/
    #pragma HLS INTERFACE m_axi port=out_testvect 
    #pragma HLS INTERFACE m_axi port=out_bara
    #pragma HLS INTERFACE m_axi port=out_barb 

    //#pragma HLS DATAFLOW

    for (int i = 0; i < N; i++) {
        in_testvect >> out_testvect[i]; 
    }
    for (int i = 0; i < n; i++) {
        in_bara >> out_bara[i]; 
    }
    in_barb >> *out_barb;
}