#include "fpga_constants.h"

extern "C" {void mulByXaiOrCopy_return(
    hls::stream<Torus32_FPGA, N>& in_testvect,
    hls::stream<Torus32_FPGA, n>& in_bara,
    Torus32_FPGA *out_testvect,
    Torus32_FPGA *out_bara 
    ) {
        /*INPUT*/
        #pragma HLS INTERFACE mode=axis port=&in_testvect depth=N
        #pragma HLS INTERFACE mode=axis port=&in_bara depth=n

        /*OUTPUT*/
        #pragma HLS INTERFACE mode=m_axi port=out_testvect
        #pragma HLS INTERFACE mode=m_axi port=out_bara

        for (int32_t i = 0; i < N; i++) {
            in_testvect >> out_testvect[i];
        }
        for (int32_t i = 0; i < n; i++) {
            in_bara >> out_bara[i];
        }
}}