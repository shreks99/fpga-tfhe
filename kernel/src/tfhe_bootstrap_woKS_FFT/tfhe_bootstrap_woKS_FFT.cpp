#include <fpga_constants.h>

int32_t modSwitchFromTorus32(Torus32_FPGA phase, int32_t Msize){
    //#pragma HLS inline
    uint64_t interv = ((UINT64_C(1)<<63)/Msize)*2; // width of each intervall
    uint64_t half_interval = interv/2; // begin of the first intervall
    uint64_t phase64 = (uint64_t(phase)<<32) + half_interval;
    //floor to the nearest multiples of interv
    return phase64/interv;
}

/*

    Splits the x input into the a and b, and switches them from Torus32. Copies the mu value across a vector.

    Config File Specs:

        in_mu       <- tfhe_bootstrap_FFT 
        in_x_a      <- tfhe_bootstrap_FFT 
        in_x_b      <- tfhe_bootstrap_FFT 
        out_testvect-> tfhe_blindRotateAndExtract_FFT 
        out_bara    -> tfhe_blindRotateAndExtract_FFT
        out_barb    -> tfhe_blindRotateAndExtract_FFT

*/

extern "C" {void tfhe_bootstrap_woKS_FFT(
    hls::stream<Torus32_FPGA>& in_mu, 
    hls::stream<Torus32_FPGA, n>& in_x_a, 
    hls::stream<Torus32_FPGA>& in_x_b,
    hls::stream<Torus32_FPGA, N>& out_testvect, 
    hls::stream<Torus32_FPGA, n>& out_bara, 
    hls::stream<Torus32_FPGA>& out_barb
    ) {
    
    /*INPUT*/
    #pragma HLS INTERFACE mode=axis port=&in_mu
    #pragma HLS INTERFACE mode=axis port=&in_x_a depth=n
    #pragma HLS INTERFACE mode=axis port=&in_x_b
    
    /*OUTPUT*/
    #pragma HLS INTERFACE mode=axis port=&out_testvect depth=N
    #pragma HLS INTERFACE mode=axis port=&out_bara depth=n
    #pragma HLS INTERFACE mode=axis port=&out_barb

    #pragma HLS DATAFLOW

    Torus32_FPGA temp_mu; 
    Torus32_FPGA temp_bara[n]; 
    Torus32_FPGA temp_barb;
    Torus32_FPGA temp_testvect[N]; 

    in_mu >> temp_mu;
    for (int i = 0; i < n; i++) in_x_a >> temp_bara[i];
    in_x_b >> temp_barb;

    temp_barb = modSwitchFromTorus32(temp_barb, N*2);
    for (int i = 0; i < n; i++) {
        //#pragma HLS UNROLL 
        temp_bara[i] = modSwitchFromTorus32(temp_bara[i], N*2);
    }

    for (int i = 0; i < N; i++) {
        //#pragma HLS UNROLL 
        temp_testvect[i] = temp_mu;
    }

    for (int i = 0; i < N; i++) out_testvect << temp_testvect[i];
    for (int i = 0; i < n; i++) out_bara << temp_bara[i];
    out_barb << temp_barb;

    return; 
}}