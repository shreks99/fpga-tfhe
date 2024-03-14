#include<fpga_constants.h>

/*

    Used just to test fuctionality of tfhe_bootstrap_FFT.cpp

    Config File Specs:

        out_bk      -> HBM/DDR
        out_mu      -> HBM/DDR
        out_x       -> HBM/DDR
        out_bk     <- tfhe_bootstrap_FFT 
        out_mu     <- tfhe_bootstrap_FFT
        out_x      <- tfhe_bootstrap_FFT

*/

extern "C" {void tfhe_bootstrap_FFT_return(
    /*INPUT*/
    // hls::stream<lweparams>& in_bk_in_out_params,
    // hls::stream<tlweparams>& in_bk_accum_params, 
    // hls::stream<tgswparams>& in_bk_bk_params, 
    // hls::stream<lweparams>& in_bk_extracted_params, 
    hls::stream<Torus32_FPGA>& in_mu, 
    hls::stream<Torus32_FPGA, N>& in_x_a, 
    hls::stream<Torus32_FPGA>& in_x_b,
    hls::stream<Torus32_FPGA>& in_x_current_variance,
    
    /*OUTPUT*/
    LweBootstrappingKeyFFT_FPGA *out_bk, 
    Torus32_FPGA *out_mu, 
    LweSample_FPGA *out_x) {

    /*INPUT INTERFACE*/
    // #pragma HLS INTERFACE mode=axis port=&in_bk_in_out_params
    // #pragma HLS INTERFACE mode=axis port=&in_bk_accum_params
    // #pragma HLS INTERFACE mode=axis port=&in_bk_bk_params
    // #pragma HLS INTERFACE mode=axis port=&in_bk_extracted_params
    #pragma HLS INTERFACE mode=axis port=&in_mu
    #pragma HLS INTERFACE mode=axis port=&in_x_a depth=N
    #pragma HLS INTERFACE mode=axis port=&in_x_b
    // #pragma HLS INTERFACE mode=axis port=&in_x_current_variance

    /*OUTPUT INTERFACE*/
    #pragma HLS INTERFACE m_axi port=out_bk bundle=gmem0
    #pragma HLS INTERFACE m_axi port=out_mu bundle=gmem2
    #pragma HLS INTERFACE m_axi port=out_x bundle=gmem3
    
    // static lweparams temp_in_out_params;
    // static ap_uint<32> temp_in_out_params_n;
    // static ap_uint<64> temp_in_out_params_alpha_min, temp_in_out_params_alpha_max;
    // static tlweparams temp_accum_params;
    // static ap_uint<32> temp_accum_params_N, temp_accum_params_k, temp_accum_params_extracted_lweparams_n;
    // static ap_uint<64> temp_accum_params_alpha_min, temp_accum_params_alpha_max, temp_accum_params_extracted_lweparams_alpha_min, temp_accum_params_extracted_lweparams_alpha_max;
    // static tgswparams temp_bk_params;
    // static ap_uint<32> temp_bk_params_l, temp_bk_params_Bgbit, temp_bk_params_Bg, temp_bk_params_halfBg, temp_bk_params_maskMod, temp_bk_params_kpl, temp_bk_params_offset;
    // static ap_uint<32> temp_bk_params_h[bk_l];
    // static ap_uint<32> temp_bk_params_N, temp_bk_params_k, temp_bk_params_extracted_lweparams_n;
    // static ap_uint<64> temp_bk_params_alpha_min, temp_bk_params_alpha_max, temp_bk_params_extracted_lweparams_alpha_min, temp_bk_params_extracted_lweparams_alpha_max;
    // static ap_uint<sizeof(tlweparams) * 8> temp_tlweparams; 
    // static lweparams temp_extracted_params; 
    
    // /*LWEPARAMS: in_out_params*/
    // in_bk_in_out_params >> temp_in_out_params;
    // temp_in_out_params_n = temp_in_out_params.data(31, 0);
    // temp_in_out_params_alpha_min = temp_in_out_params.data(95, 32);
    // temp_in_out_params_alpha_max = temp_in_out_params.data(159, 96);
    // out_bk->in_out_params.n = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_in_out_params_n));
    // out_bk->in_out_params.alpha_min = *reinterpret_cast<double*>(reinterpret_cast<uint64_t*>(&temp_in_out_params_alpha_min));
    // out_bk->in_out_params.alpha_max = *reinterpret_cast<double*>(reinterpret_cast<uint64_t*>(&temp_in_out_params_alpha_max));

    // /*TLWEPARAMS: accum_params*/
    // in_bk_accum_params >> temp_accum_params;
    // temp_accum_params_N = temp_accum_params.data(31, 0);
    // temp_accum_params_k = temp_accum_params.data(63, 32);
    // temp_accum_params_alpha_min = temp_accum_params.data(127, 64);
    // temp_accum_params_alpha_max = temp_accum_params.data(191, 128);
    // temp_accum_params_extracted_lweparams_n = temp_accum_params.data(223, 192);
    // temp_accum_params_extracted_lweparams_alpha_min = temp_accum_params.data(287, 224);
    // temp_accum_params_extracted_lweparams_alpha_max = temp_accum_params.data(351, 288);
    // out_bk->accum_params.N = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_accum_params_N));
    // out_bk->accum_params.k = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_accum_params_k));
    // out_bk->accum_params.alpha_min = *reinterpret_cast<double*>(reinterpret_cast<uint64_t*>(&temp_accum_params_alpha_min));
    // out_bk->accum_params.alpha_max = *reinterpret_cast<double*>(reinterpret_cast<uint64_t*>(&temp_accum_params_alpha_max));
    // out_bk->accum_params.extracted_lweparams.n = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_accum_params_extracted_lweparams_n));
    // out_bk->accum_params.extracted_lweparams.alpha_min = *reinterpret_cast<double*>(reinterpret_cast<uint64_t*>(&temp_accum_params_extracted_lweparams_alpha_min));
    // out_bk->accum_params.extracted_lweparams.alpha_max = *reinterpret_cast<double*>(reinterpret_cast<uint64_t*>(&temp_accum_params_extracted_lweparams_alpha_max));

    // /*TGSWPARAMS: bk_params*/
    // in_bk_bk_params >> temp_bk_params; 
    // temp_bk_params_l = temp_bk_params.data(31, 0);
    // temp_bk_params_Bgbit = temp_bk_params.data(63, 32);
    // temp_bk_params_Bg = temp_bk_params.data(95, 64);
    // temp_bk_params_halfBg = temp_bk_params.data(127, 96);
    // temp_bk_params_maskMod = temp_bk_params.data(159, 128);
    // // temp_bk_params_N = temp_bk_params.data(191, 160);
    // // temp_bk_params_k = temp_bk_params.data(223, 192);
    // // temp_bk_params_alpha_min = temp_bk_params.data(287, 224);
    // // temp_bk_params_alpha_max = temp_bk_params.data(351, 288);
    // // temp_bk_params_extracted_lweparams_n = temp_bk_params.data(383ß, 352);
    // // temp_bk_params_extracted_lweparams_alpha_min = temp_bk_params.data(447, 384);
    // // temp_bk_params_extracted_lweparams_alpha_max = temp_bk_params.data(511, 448);
    // temp_tlweparams = temp_bk_params.data(511, 160);
    // temp_bk_params_kpl = temp_bk_params.data(543, 512);
    // temp_bk_params_h[0] = temp_bk_params.data(575, 544);
    // temp_bk_params_h[1] = temp_bk_params.data(607, 576);
    // temp_bk_params_h[2] = temp_bk_params.data(639, 608);
    // temp_bk_params_offset = temp_bk_params.data(671, 640);
    // out_bk->bk_params.l = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_l));
    // out_bk->bk_params.Bgbit = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_Bgbit));
    // out_bk->bk_params.Bg = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_Bg));
    // out_bk->bk_params.halfBg = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_halfBg));
    // out_bk->bk_params.maskMod = *reinterpret_cast<uint32_t*>(&temp_bk_params_maskMod);
    // out_bk->bk_params.tlwe_params = *reinterpret_cast<TLweParams_FPGA*>(reinterpret_cast<ap_uint<sizeof(tgswparams) * 8>*>(&temp_tlweparams));
    // out_bk->bk_params.kpl = *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_kpl));
    // out_bk->bk_params.h[0] =  *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_h[0]));
    // out_bk->bk_params.h[1] =  *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_h[1]));
    // out_bk->bk_params.h[2] =  *reinterpret_cast<int32_t*>(reinterpret_cast<uint32_t*>(&temp_bk_params_h[2]));
    // out_bk->bk_params.offset = *reinterpret_cast<uint32_t*>(&temp_bk_params_offset);

    // /*LWEPARAMS: extracted_params*/
    // in_bk_extracted_params >> temp_extracted_params; 
    // out_bk->extract_params = *reinterpret_cast<LweParams_FPGA*>(reinterpret_cast<ap_uint<sizeof(lweparams) * 8>*>(&temp_extracted_params));

    in_mu >> *out_mu; 

    for (int i = 0; i < N; i++) {
        in_x_a >> out_x->a[i];
    }
    in_x_b >> out_x->b; 
    return; 
}}

