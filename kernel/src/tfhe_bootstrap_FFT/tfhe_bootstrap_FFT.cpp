#include<fpga_constants.h>

/*

    Starting function of TFHE (possibly not needed). The key, mu, and x value are passed from the HOST to MEM and read by this
    and written into stream to start the computations. 

    Config File Specs:

        in_bk      <- HBM/DDR
        in_mu      <- HBM/DDR
        in_x       <- HBM/DDR
        out_mu     -> tfhe_bootstrap_woKS_FFT 
        out_x_a    -> tfhe_bootstrap_woKS_FFT
        out_x_b    -> tfhe_bootstrap_woKS_FFT

*/

extern "C" {void tfhe_bootstrap_FFT(
    /*INPUT*/
    //LweBootstrappingKeyFFT_FPGA *in_bk, 
    Torus32_FPGA *in_mu, 
    LweSample_FPGA *in_x,
    
    /*OUTPUT*/
    // hls::stream<lweparams>& out_bk_in_out_params,
    // hls::stream<tlweparams>& out_bk_accum_params,
    // hls::stream<tgswparams>& out_bk_bk_params, 
    // hls::stream<lweparams>& out_bk_extracted_params,  

    hls::stream<Torus32_FPGA>& out_mu, 
    hls::stream<Torus32_FPGA, n>& out_x_a, 
    hls::stream<Torus32_FPGA>& out_x_b
    //hls::stream<Torus32_FPGA>& out_x_current_variance
    ) {

    /*INPUT INTERFACE*/
    //#pragma HLS INTERFACE m_axi port=in_bk bundle=gmem0
    #pragma HLS INTERFACE m_axi port=in_mu bundle=gmem1
    #pragma HLS INTERFACE m_axi port=in_x

    /*OUTPUT INTERFACE*/
    // #pragma HLS INTERFACE mode=axis port=&out_bk_in_out_params
    // #pragma HLS INTERFACE mode=axis port=&out_bk_accum_params
    // #pragma HLS INTERFACE mode=axis port=&out_bk_bk_params
    // #pragma HLS INTERFACE mode=axis port=&out_bk_extracted_params
    #pragma HLS INTERFACE mode=axis port=&out_mu
    #pragma HLS INTERFACE mode=axis port=&out_x_a depth=n
    #pragma HLS INTERFACE mode=axis port=&out_x_b
    // #pragma HLS INTERFACE mode=axis port=&out_x_current_variance

    //#pragma HLS DATAFLOW

    // static lweparams temp_in_out_params, temp_extracted_params; 
    // static tlweparams temp_accum_params; 
    // static tgswparams temp_bk_params; 

    // /*LWEPARAMS: in_out_params*/
    // temp_in_out_params.data = *reinterpret_cast<ap_uint<sizeof(lweparams) * 8>*>(&(in_bk->in_out_params));
    // out_bk_in_out_params << temp_in_out_params;

    // /*TLWEPARAMS: accum_params*/
    // temp_accum_params.data = *reinterpret_cast<ap_uint<sizeof(tlweparams) * 8>*>(&(in_bk->accum_params));
    // out_bk_accum_params << temp_accum_params; 

    // /*TGSWPARAMS: bk_params*/
    // temp_bk_params.data = *reinterpret_cast<ap_uint<sizeof(tgswparams) * 8>*>(&(in_bk->bk_params));
    // out_bk_bk_params << temp_bk_params; 

    // /*LWEPARAMS: extracted_params*/
    // temp_extracted_params.data = *reinterpret_cast<ap_uint<sizeof(lweparams) * 8>*>(&(in_bk->extract_params));
    // out_bk_extracted_params << temp_extracted_params;    

    /*MU*/
    out_mu << *in_mu; 

    /*X*/
    for (int i = 0; i < n; i++) {
        out_x_a << in_x->a[i];
    }
    out_x_b << in_x->b; 
    //out_x_current_variance.write(in_x->current_variance); 

    return; 
}}
