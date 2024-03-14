#include "fpga_constants.h"

extern "C" {void mulByXaiOrCopy(
    hls::stream<Torus32_FPGA, N>& in_v, 
    hls::stream<Torus32_FPGA>& in_barb,
    hls::stream<Torus32_FPGA, N>& out_testvect
    ) {

        /*INPUT*/
        #pragma HLS INTERFACE mode=axis port=&in_v depth=N
        #pragma HLS INTERFACE mode=axis port=&in_barb

        /*OUTPUT*/
        #pragma HLS INTERFACE mode=axis port=&out_testvect depth=N

        //#pragma HLS DATAFLOW

        Torus32_FPGA temp_barb; 
        Torus32_FPGA temp_testvect[N];
        Torus32_FPGA temp_v[N];

        in_barb >> temp_barb;
        for (int32_t i = 0; i < N; i++) in_v >> temp_v[i];

        if (temp_barb != 0) {
            int32_t a = 2 * N - temp_barb; 
            if (a < N) {
                for (int32_t i = 0; i < a; i++)//sur que i-a<0
                    temp_testvect[i] = -temp_v[i - a + N];
                for (int32_t i = a; i < N; i++)//sur que N>i-a>=0
                    temp_testvect[i] = temp_v[i - a];
            } else {
                int32_t aa = a - N;
                for (int32_t i = 0; i < aa; i++)//sur que i-a<0
                    temp_testvect[i] = temp_v[i - aa + N];
                for (int32_t i = aa; i < N; i++)//sur que N>i-a>=0
                    temp_testvect[i] = -temp_v[i - aa];
            }
        } else {
            for (int32_t i = 0; i < N; i++) temp_testvect[i] = temp_v[i];
        }
        
        for (int32_t i = 0; i < N; i++) out_testvect << temp_testvect[i];
}}