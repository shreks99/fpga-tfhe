// SPIRAL License
//
// Copyright 2017, Carnegie Mellon University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the SPIRAL project.

#include <math.h>
#include "ap_int.h"
#include "header.h"

template <typename T>
void switch_2_by_2(T in[2], T out[2], bool switch_on) {
	out[0] = in[(switch_on==true)?(1):(0)];
	out[1] = in[(switch_on==true)?(0):(1)];
}

template <typename T>
void spatial_permutation(T spat_in[SW], T spat_out[SW], int y_idx[SW]) {
	int i;
	for (i=0; i<SW; i++) {
		spat_out[y_idx[i]] = spat_in[i];
	}
}

template <typename T>
void copy_vector(T from[SW], T to[SW]) {
	int i;
	for (i=0; i<SW; i++) {
		to[i] = from[i];
	}
}

template <typename T>
void connect_to_switch(T con_to_swh_in[SW], T con_to_swh_out[SW/2][2], int x_idx[SW]) {
#pragma HLS ARRAY_PARTITION variable=con_to_swh_in,con_to_swh_out,x_idx complete dim=1
#pragma HLS ARRAY_PARTITION variable=con_to_swh_out complete dim=2
#pragma HLS INLINE
	int i,j;
	for (i=0; i<SW/2; i++) {
#pragma HLS UNROLL
		for (j=0; j<2; j++) {
			con_to_swh_out[i][j] = con_to_swh_in[x_idx[2*i+j]];
		}
	}
}

template <typename T>
void connect_from_switch(T con_from_swh_in[SW/2][2], T con_from_swh_out[SW], int y_idx[SW]) {
#pragma HLS ARRAY_PARTITION variable=con_from_swh_in,con_from_swh_out,y_idx complete dim=1
#pragma HLS ARRAY_PARTITION variable=con_from_swh_in complete dim=2
#pragma HLS INLINE
	int i,j;
	for (i=0; i<SW/2; i++) {
#pragma HLS UNROLL
		for (j=0; j<2; j++) {
			con_from_swh_out[y_idx[2*i+j]] = con_from_swh_in[i][j];
		}
	}
}

template <typename T>
void switch_array(int num, T swh_array_in[SW/2][2], T swh_array_out[SW/2][2], bool switch_on) {
	int i;
	for (i=0; i<num; i++) {
#pragma HLS UNROLL
		switch_2_by_2 <T> (swh_array_in[i], swh_array_out[i], switch_on);
	}
}

template <typename T>
void onestage(T PRE_IN[SW], T NEXT_PRE_IN[SW],  bool control, int idx_in_or_out[SW]) {
#pragma HLS INLINE
    T IN[SW/2][2], OUT[SW/2][2];

    connect_to_switch <T> (PRE_IN, IN, idx_in_or_out);
    switch_array <T> (SW/2, IN, OUT, control);
    connect_from_switch <T> (OUT, NEXT_PRE_IN, idx_in_or_out);
}

template <typename T, int NUM_STAGE>
void switch_network_write(T X[SW], T Y[SW], ap_uint<LOG2N-LOG2SW> bits, int init_perm_idx[SW], int idx_in_or_out[NUM_STAGE][SW], int control_bit[NUM_STAGE]) {
#pragma HLS ARRAY_PARTITION variable=X,Y complete dim=1
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INLINE off

	T PRE_IN[NUM_STAGE+1][SW];

	// initial permutation
	spatial_permutation <T> (X, PRE_IN[0], init_perm_idx);

	int i;
	for (i=0; i<NUM_STAGE; i++) {
#pragma HLS UNROLL
	    onestage <T> (PRE_IN[i], PRE_IN[i+1], bits.get_bit(control_bit[i]-LOG2SW), idx_in_or_out[i]);
	}

	// output port
	copy_vector <T> (PRE_IN[NUM_STAGE], Y);
}

void buf_write_addr_generation(ap_uint<LOG2N-LOG2SW> in_count, bool flip, int bit_seq[LOG2N-LOG2SW], ap_uint<LOG2N-LOG2SW+1> widx[2]) {
#pragma HLS INLINE
	int i, j;
	for (i=0; i<SW; i++) {
	#pragma HLS UNROLL
		ap_uint<LOG2N> bits = SW*in_count+i;

		for (j=0; j<LOG2N-LOG2SW; j++) {
	#pragma HLS UNROLL
			bool bit = bits.get_bit( bit_seq[LOG2N-LOG2SW-1-j] );
			widx[i].set_bit(j, bit);
		}
		widx[i].set_bit(LOG2N-LOG2SW, flip);
	}
}

void buf_read_addr_generation(ap_uint<LOG2N-LOG2SW> out_count, bool flip, ap_uint<LOG2N-LOG2SW+1> ridx[2]) {
	int i;
	for (i=0; i<SW; i++) {
#pragma HLS UNROLL
		ridx[i].range(LOG2N-LOG2SW-1, 0) = out_count.range(LOG2N-LOG2SW-1, 0);
		ridx[i].set_bit(LOG2N-LOG2SW, flip);
	}
}

template <typename T, int NUM_STAGE>
void switch_network_read(T X[SW], T Y[SW], ap_uint<LOG2N-LOG2SW> bits, int idx_in_or_out[NUM_STAGE][SW], int control_bit[NUM_STAGE]) {
#pragma HLS ARRAY_PARTITION variable=X,Y complete dim=1
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INLINE off

	T PRE_IN[NUM_STAGE+1][SW];

//	// no initial permutation for the read switch network.
	copy_vector <T> (X, PRE_IN[0]);

	int i;
	for (i=0; i<NUM_STAGE; i++) {
#pragma HLS UNROLL
	    onestage <T> (PRE_IN[i], PRE_IN[i+1], bits.get_bit(control_bit[i]-LOG2SW), idx_in_or_out[i]);
	}

	// output port
	copy_vector <T> (PRE_IN[NUM_STAGE], Y);
}

void buf_write(cmplx_type buf[SW][NPEASE*2/SW], content_addr in_post_switch[SW]) {
	// buf write
	int i;
	for (i=0; i<SW; i++) {
#pragma HLS UNROLL
		buf[i][in_post_switch[i].addr] = in_post_switch[i].data;
	}
}

void buf_read(cmplx_type buf[SW][NPEASE*2/SW], cmplx_type in_from_buff_pre_switch[SW], ap_uint<LOG2N-LOG2SW+1> ridx_post_switch[SW]) {
	// buf read
	int i;
	for (i=0; i<SW; i++) {
//#pragma HLS UNROLL
		in_from_buff_pre_switch[i] = buf[i][ridx_post_switch[i]];
	}
}

void combine_addr_data(content_addr combination[SW], ap_uint<LOG2N-LOG2SW+1> addr[SW],
		cmplx_type data[SW]) {
	int i;
	for (i = 0; i < SW; i++) {
#pragma HLS UNROLL
		combination[i].data = data[i];
		combination[i].addr = addr[i];
	}
}


void fixed_point_scale_after_dft(cmplx_type x[Ri], cmplx_type y[Ri], ap_uint<LOG2_LOGrN> i__) {
	int i;
	for (i=0; i<Ri; i++) {

//		y[i].real = x[i].real >> ((i__!=9)?LOG2R:3);
//		y[i].imag = x[i].imag >> ((i__!=9)?LOG2R:3);
//		y[i].real = x[i].real ;
//		y[i].imag = x[i].imag ;
		y[i].real = x[i].real / 4; //exp2(LOG2R);
		y[i].imag = x[i].imag / 4; //exp2(LOG2R);
	}
}

#if Ri == 2
void dft(cmplx_type x[2], cmplx_type y[2]) {
	CMPXADD(y[0], x[0], x[1]);
	CMPXSUB(y[1], x[0], x[1]);
}

#elif Ri == 4
void dft(cmplx_type x[4], cmplx_type y[4]) {
	cmplx_type tmp[4];

	CMPXADD(tmp[0], x[0], x[2]);
	CMPXSUB(tmp[1], x[0], x[2]);
	CMPXADD(tmp[2], x[1], x[3]);
//	CMPXSUB_MUL_NI(tmp[3], x[1], x[3]);
	CMPX_NI_MUL_X_SUB_Y(tmp[3], x[1], x[3]);

	CMPXADD(y[0], tmp[0], tmp[2]);
	CMPXSUB(y[2], tmp[0], tmp[2]);
	CMPXADD(y[1], tmp[1], tmp[3]);
	CMPXSUB(y[3], tmp[1], tmp[3]);

}

#elif Ri == 8
void dft(cmplx_type X[8], cmplx_type Y[8]) {
    cmplx_type s16, s17, s18, s19, s20, t66, t67, t68
            , t69, t70, t71, t72, t73, t74, t75, t76;
    CMPXADD(t66, X[0], X[4]);
    CMPXSUB(t67, X[0], X[4]);
    CMPXADD(t68, X[1], X[5]);
    CMPX_TWR_NTWR_MUL_X_SUB_Y(s16, 0.70710678118654757, X[1], X[5]);
    CMPXADD(t69, X[2], X[6]);
    CMPX_NI_MUL_X_SUB_Y(s17, X[2], X[6]);
    CMPXADD(t70, X[3], X[7]);
    CMPX_TWR_TWR_MUL_X_SUB_Y(s18, -0.70710678118654757, X[3], X[7]);
    CMPXADD(t71, t66, t69);
    CMPXSUB(t72, t66, t69);
    CMPXADD(t73, t68, t70);
    CMPX_NI_MUL_X_SUB_Y(s19, t68, t70);
    CMPXADD(Y[0], t71, t73);
    CMPXSUB(Y[4], t71, t73);
    CMPXADD(Y[2], t72, s19);
    CMPXSUB(Y[6], t72, s19);
    CMPXADD(t74, t67, s17);
    CMPXSUB(t75, t67, s17);
    CMPXADD(t76, s16, s18);
    CMPX_NI_MUL_X_SUB_Y(s20, s16, s18);
    CMPXADD(Y[1], t74, t76);
    CMPXSUB(Y[5], t74, t76);
    CMPXADD(Y[3], t75, s20);
    CMPXSUB(Y[7], t75, s20);

}
#endif


void convert_2d_to_1d(cmplx_type X[SW/Ri][Ri], cmplx_type Y[SW]) {
	int i,j;
	for (i=0; i<SW/Ri; i++) {
		for (j=0; j<Ri; j++) {
			Y[Ri*i+j] = X[i][j];
		}
	}
}

void convert_1d_to_2d(cmplx_type X[SW], cmplx_type Y[SW/Ri][Ri]) {
	int i,j;
	for (i=0; i<SW/Ri; i++) {
		for (j=0; j<Ri; j++) {
			Y[i][j] = X[Ri*i+j];
		}
	}
}

void dft_bundle_module_old(cmplx_type dft_in[SW], cmplx_type dft_out[SW]) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INLINE off
#pragma HLS PIPELINE

	cmplx_type data_dft_in[SW/Ri][Ri], data_dft_out[SW/Ri][Ri];
	cmplx_type data_scaled[SW/Ri][Ri];

	convert_1d_to_2d(dft_in, data_dft_in);

	int i;
	for (i=0; i<SW/Ri; i++) {
#pragma HLS UNROLL
		dft(data_dft_in[i], data_dft_out[i]);
#ifdef FIXED_POINT
//		fixed_point_scale_after_dft(data_dft_out[i], data_scaled[i]);
#endif
	}

#ifdef FIXED_POINT
	convert_2d_to_1d(data_scaled, dft_out);
#else
	convert_2d_to_1d(data_dft_out, dft_out);
#endif
}


void dft_bundle_module(cmplx_type dft_in[SW], cmplx_type dft_out[SW], ap_uint<LOG2_LOGrN> i__) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INLINE off
#pragma HLS PIPELINE

	cmplx_type data_dft_in[SW/Ri][Ri], data_dft_out[SW/Ri][Ri];
	cmplx_type data_scaled[SW/Ri][Ri];

	convert_1d_to_2d(dft_in, data_dft_in);

	int i;
	for (i=0; i<SW/Ri; i++) {
#pragma HLS UNROLL
		dft(data_dft_in[i], data_dft_out[i]);
#ifdef FIXED_POINT
		fixed_point_scale_after_dft(data_dft_out[i], data_scaled[i], i__);
#endif
	}

#ifdef FIXED_POINT
	convert_2d_to_1d(data_scaled, dft_out);
#else
	convert_2d_to_1d(data_dft_out, dft_out);
#endif
}

void twidscale_module(cmplx_type data_pre_twid[SW], cmplx_type data_post[SW], ap_uint<LOG2_LOGrN> i) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INLINE off
#pragma HLS PIPELINE

	const VAL_TYPE twid_table_R[NPEASE] ={
#ifdef FIXED_POINT
	#include "twidR_fixed_point.txt"
#else
	#include "twidR.txt"
#endif
	};
//#pragma HLS RESOURCE variable=twid_table_R core=ROM_nP_BRAM
//#pragma HLS ARRAY_MAP variable=twid_table_R instance=twid_RI vertical
	const VAL_TYPE twid_table_I[NPEASE] ={
#ifdef FIXED_POINT
	#include "twidI_fixed_point.txt"
#else
	#include "twidI.txt"
#endif
	};
//#pragma HLS RESOURCE variable=twid_table_I core=ROM_nP_BRAM
//#pragma HLS ARRAY_MAP variable=twid_table_I instance=twid_RI vertical

	static ap_uint<LOG2N-LOG2SW> j_count = NPEASE/SW-1;

	if (j_count==NPEASE/SW-1) {
		j_count = 0;
	}
	else {
		j_count++;
	}


	ap_uint<LOG2N> twid_idx_ofset;
	twid_idx_ofset.range(LOG2R-1, 0) = Ri-1;
	twid_idx_ofset.range(LOG2N-1, LOG2R) = (NPEASE/Ri-1) << ( LOG2R*(LOGrN-1-i)  );

	int l,j;
	ap_uint<LOG2N> ld_idx_base = SW*j_count;

	for (j=0; j<SW/Ri; j++) {
		data_post[Ri*j + 0] = data_pre_twid[Ri*j + 0];

		for (l=1; l<Ri; l++) {
			int ld_idx = ld_idx_base + j*Ri + l;
			cmplx_type twid;
			twid.real = twid_table_R[ld_idx & twid_idx_ofset];
			twid.imag = twid_table_I[ld_idx & twid_idx_ofset];

			CMPXMUL(data_post[j*Ri + l], twid, data_pre_twid[j*Ri + l]);
		}
	}
}





#define LATENCY 46
#define LOG_LATENCY 6


void digit_rev(unsigned int in, unsigned int* out, unsigned int bitwid) {
#pragma HLS INLINE
	int j;
	ap_uint<LOG2R> dummy1 = ~0;
	unsigned int idx = 0;
	for (j=0; j<bitwid; j+=LOG2R) {
#pragma HLS UNROLL
		idx = Ri*idx + ((in>>j)&dummy1);
	}
//	*out = (bitwid==0)?in:idx;
	*out = idx;
}




#if SW*SW<=NPEASE
//lighter compute
void digit_rev_perm_N(cmplx_type x[NPEASE],cmplx_type y[NPEASE]) {
#pragma HLS INLINE off
DO_PRAGMA(HLS ARRAY_PARTITION variable=x cyclic factor=SW dim=1)
DO_PRAGMA(HLS ARRAY_PARTITION variable=y cyclic factor=SW dim=1)
#pragma HLS RESOURCE variable=x,y core=RAM_S2P_BRAM
	unsigned int i;
	unsigned int in_mod, in_mul, out_prt;
	unsigned int out_mod, out_mul, in_prt;
	int in_idx, out_idx;
	cycle_modulo: for (in_mod=0; in_mod<NPEASE/SW/SW; in_mod++) {
DO_PRAGMA(HLS PIPELINE II=SW rewind)
		digit_rev(in_mod, &out_mod, LOG2N - 2*LOG2SW);
		in_prt=0; out_prt=0;
		cycle_multiplier_prt: for (i=0; i<SW*SW; i++) {
			out_prt = i%SW;
//			in_prt = (out_prt + i>>LOGSW)%SW; HLS DOESN'T PIPELINE WELL WITH THIS.

			digit_rev(out_prt, &in_mul, LOG2SW);
			digit_rev(in_prt, &out_mul, LOG2SW);

			in_idx = (in_mul*NPEASE/SW) + (in_mod*SW) + in_prt;
			out_idx = (out_mul*NPEASE/SW) + (out_mod*SW) + out_prt;

//			printf("in_mul=%d, in_mod=%d, in_prt=%d\n", in_mul, in_mod, in_prt);
//			printf("ou_mul=%d, ou_mod=%d, ou_prt=%d\n", out_mul, out_mod, out_prt);
//			printf("x[%2d] -> y[%2d], port: [%d] -> [%d]\n", in_idx, out_idx, in_idx%SW, out_idx%SW);

			y[out_idx] = x[in_idx];

//			out_prt = (out_prt+1)%SW;
			in_prt = (in_prt+1)%SW;
			if (i%SW==SW-1)
				in_prt = (in_prt+1)%SW;
		}
	}
}

#else
void digit_rev_perm_N(cmplx_type x[NPEASE],cmplx_type y[NPEASE]) {
DO_PRAGMA(HLS PIPELINE II=N_O_SW rewind)
DO_PRAGMA(HLS ARRAY_PARTITION variable=x cyclic factor=SW dim=1)
DO_PRAGMA(HLS ARRAY_PARTITION variable=y cyclic factor=SW dim=1)
#pragma HLS RESOURCE variable=x,y core=RAM_S2P_BRAM
	unsigned int i, in_mul, out_prt, pad, pad_rev, out_prt_rev;
//	ap_uint<LOGN-LOGSW> pad;

//	unsigned int mask = (1<<(2*LOGSW-LOGN))-1;

	int in_idx, out_idx;
	padd: for (i=0; i<NPEASE/SW; i++) {
// doesn't work because modulo scheduling is impossible. (port in -> port out pairs must be the same for each iteration)
//DO_PRAGMA(HLS PIPELINE II=SW rewind)
		pad = i;
		all_prts: for (out_prt=0; out_prt<SW; out_prt++) {
			digit_rev(pad, &pad_rev, LOGrN-LOG2SW);
			digit_rev(out_prt, &out_prt_rev, LOG2SW);

//			unsigned int in_prt = ( (out_prt_rev & mask) <<(LOGN-LOGSW) ) + pad_rev;
////			printf("in_prt2 = %d\n", in_prt); // correct
////			unsigned int in_idx2;
//			in_idx = ((out_prt_rev>>(2*LOGSW-LOGN))<<LOGSW) + in_prt;
////			printf("in_idx2 = %d\n", in_idx2);

			in_idx = (out_prt_rev *NPEASE/SW) + pad_rev;
			out_idx = (pad *SW) + out_prt;
//			printf("x[%2d] -> y[%2d], port: [%d] -> [%d]\n", in_idx, out_idx, in_idx%SW, out_idx%SW);
			y[out_idx] = x[in_idx];

			pad = (pad+1)% (NPEASE/SW);
		}
	}
}
#endif

void digit_rev_perm_switch_write(cmplx_type data_out_switched[SW], cmplx_type bram[SW][NPEASE*2/SW], bool wt_offset) {
	static ap_uint<LOG2N-LOG2SW> j_com = NPEASE/SW-1;
	if (j_com==NPEASE/SW-1) {
		j_com = 0;
	}
	else {
		j_com++;
	}

	perm_config<DIGIT_REV_NUM_STAGE> config = {
#include "dig_rev_perm_config.dat"
	};

	// buf write addr generation
	ap_uint<LOG2N-LOG2SW+1> w_addr[SW];

	buf_write_addr_generation(j_com, wt_offset, config.w_addr_bit_seq, w_addr);



	// input switch (addr + data)
	// compose the combination of write addr and data.
	content_addr in_pre_switch[SW], in_post_switch[SW];
	combine_addr_data(in_pre_switch, w_addr, data_out_switched);

	switch_network_write <content_addr, DIGIT_REV_NUM_STAGE> (in_pre_switch, in_post_switch, j_com, config.init_perm_idx, config.w_switch_connection_idx, config.w_switch_control_bit);

	// buf write
	buf_write(bram, in_post_switch);
}


void digit_rev_perm_read_switch(cmplx_type bram[SW][NPEASE*2/SW], bool rd_offset, bool first_stage, cmplx_type data_in[SW]) {
	static ap_uint<LOG2N-LOG2SW> j_readbuf = NPEASE/SW-1;

	if (j_readbuf==NPEASE/SW-1) {
		j_readbuf = 0;
	}
	else {
		j_readbuf++;
	}

	perm_config<DIGIT_REV_NUM_STAGE> config = {
#include "dig_rev_perm_config.dat"
	};

	// buf read addr generation
	ap_uint<LOG2N-LOG2SW+1> r_addr[SW], r_addr_post_switch[SW];
	buf_read_addr_generation(j_readbuf, rd_offset, r_addr);

	// switch the read address
	//  (actually unnecessary because they are always the same!!)

	// buf read
	cmplx_type in_from_buff_pre_switch[SW];
	buf_read(bram, in_from_buff_pre_switch, r_addr);

	// switch out data
	if (first_stage) {
		copy_vector(in_from_buff_pre_switch, data_in);
	}
	else {
		switch_network_read <cmplx_type, DIGIT_REV_NUM_STAGE>(in_from_buff_pre_switch, data_in, j_readbuf, config.r_switch_connection_idx, config.r_switch_control_bit);
	}
}

void digrev_or_stride_perm_read_switch(cmplx_type bram[SW][NPEASE*2/SW], bool rd_offset, bool first_stage, cmplx_type data_in[SW]) {
	static ap_uint<LOG2N-LOG2SW> j_readbuf = NPEASE/SW-1;

	if (j_readbuf==NPEASE/SW-1) {
		j_readbuf = 0;
	}
	else {
		j_readbuf++;
	}

	perm_config<STRIDE_PERM_SWITCH_NUM_STAGE> config = {
#include "stride_perm_config.dat"
	};
	perm_config<DIGIT_REV_NUM_STAGE> dig_rev_config = {
#include "dig_rev_perm_config.dat"
	};

	// buf read addr generation
	ap_uint<LOG2N-LOG2SW+1> r_addr[SW], r_addr_post_switch[SW];
	buf_read_addr_generation(j_readbuf, rd_offset, r_addr);

	// switch the read address
	//  (actually unnecessary because they are always the same!!)

	// buf read
	cmplx_type in_from_buff_pre_switch[SW];
	buf_read(bram, in_from_buff_pre_switch, r_addr);

	// switch out data
	if (first_stage) {
		copy_vector(in_from_buff_pre_switch, data_in);
		switch_network_read <cmplx_type, DIGIT_REV_NUM_STAGE>(in_from_buff_pre_switch, data_in, j_readbuf, dig_rev_config.r_switch_connection_idx, dig_rev_config.r_switch_control_bit);
	}
	else {
		switch_network_read <cmplx_type, STRIDE_PERM_SWITCH_NUM_STAGE>(in_from_buff_pre_switch, data_in, j_readbuf, config.r_switch_connection_idx, config.r_switch_control_bit);
	}
}

void stride_perm_switch_write(cmplx_type data_out_switched[SW], cmplx_type bram[SW][NPEASE*2/SW], bool wt_offset) {
	static ap_uint<LOG2N-LOG2SW> j_com = NPEASE/SW-1;
	if (j_com==NPEASE/SW-1) {
		j_com = 0;
	}
	else {
		j_com++;
	}

	perm_config<STRIDE_PERM_SWITCH_NUM_STAGE> config = {
#include "stride_perm_config.dat"
	};

	// buf write addr generation
	ap_uint<LOG2N-LOG2SW+1> w_addr[SW];

	buf_write_addr_generation(j_com, wt_offset, config.w_addr_bit_seq, w_addr);


	// input switch (addr + data)
	// compose the combination of write addr and data.
	content_addr in_pre_switch[SW], in_post_switch[SW];
	combine_addr_data(in_pre_switch, w_addr, data_out_switched);

	switch_network_write <content_addr, STRIDE_PERM_SWITCH_NUM_STAGE> (in_pre_switch, in_post_switch, j_com, config.init_perm_idx, config.w_switch_connection_idx, config.w_switch_control_bit);

	// buf write
	buf_write(bram, in_post_switch);
}

void final_stride_perm_switch(cmplx_type bram[SW][NPEASE*2/SW], bool rd_offset, cmplx_type output[SW]) {
	bool wt_offset = !rd_offset;
	static ap_uint<LOG2N-LOG2SW> j_readbuf = NPEASE/SW-1;

	if (j_readbuf==NPEASE/SW-1) {
		j_readbuf = 0;
	}
	else {
		j_readbuf++;
	}

	perm_config<STRIDE_PERM_SWITCH_NUM_STAGE> config = {
#include "stride_perm_config.dat"
	};

	// buf read addr generation
	ap_uint<LOG2N-LOG2SW+1> r_addr[SW], r_addr_post_switch[SW];
	buf_read_addr_generation(j_readbuf, rd_offset, r_addr);

	// switch the read address
	//  (actually unnecessary because they are always the same!!)

	// buf read
	cmplx_type in_from_buff_pre_switch[SW], data_in[SW];
	buf_read(bram, in_from_buff_pre_switch, r_addr);

	// switch out data
	switch_network_read <cmplx_type, STRIDE_PERM_SWITCH_NUM_STAGE>(in_from_buff_pre_switch, output, j_readbuf, config.r_switch_connection_idx, config.r_switch_control_bit);


}

extern "C" void pease_fft(cmplx_type X[NPEASE], cmplx_type Y[NPEASE]) {
#pragma HLS DATA_PACK variable=X
#pragma HLS DATA_PACK variable=Y
#pragma HLS INTERFACE axis port=X
#pragma HLS INTERFACE axis port=Y
DO_PRAGMA(HLS ARRAY_PARTITION variable=X cyclic factor=SW dim=1)
DO_PRAGMA(HLS ARRAY_PARTITION variable=Y cyclic factor=SW dim=1)

	int i;                          // loop iterator for Log(Ri)NPEASE stages
	int j;                          // loop iterator for NPEASE/SW butterfly bundles
	int k;                          // loop iterator for SW/Ri butterflies
	cmplx_type in[SW];     			// temporary variable for data stream in
	cmplx_type data_in[SW];  		// temporary variable for data pre-twiddled
	cmplx_type data_twiddled[SW]; 	// temporary variable for data post-twiddled
	cmplx_type data_out[SW]; 		// temporary variable for data post base dft
	cmplx_type out[SW];   			// temporary variable for data stream out
	cmplx_type buf[SW][NPEASE*2/SW];     // buffer for iterative calculation (num of ports == SW; size == 2N)
//#pragma HLS DATA_PACK variable=buf
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
	bool digit_rev_flip;            // whether to flip BRAM when writing for digit-rev permutation.
	bool rd_flip, wt_flip;          // whether to flip BRAM for Ri/W for stride permutation.
	bool final_rd_flip;             // whether to flip BRAM for Ri for the final stride permutation.

	digit_rev_flip= false;          // write streamed in data to BRAM without addr offset
	for (j=0; j<NPEASE/SW; j++) {
#pragma HLS PIPELINE
		for (k=0; k<SW; k++) {
			in[k] = X[SW*j+k];
		}
		digit_rev_perm_switch_write(in, buf, digit_rev_flip);
	}

	for (i=0; i<LOGrN; i++) {
		for (j=0; j<NPEASE/SW; j++) {
#pragma HLS DEPENDENCE variable=buf inter false
#pragma HLS PIPELINE
			rd_flip = ((i%2==0)?(false):(true));
			wt_flip = !rd_flip;

			digrev_or_stride_perm_read_switch(buf, rd_flip, i==0, data_in);

			twidscale_module(data_in, data_twiddled, i);
			dft_bundle_module(data_twiddled, data_out, i);

			stride_perm_switch_write(data_out, buf, wt_flip);
		}
	}

	final_rd_flip = ((LOGrN%2==0)?(false):(true));
	for (j=0; j<NPEASE/SW; j++) {
#pragma HLS PIPELINE
		final_stride_perm_switch(buf, final_rd_flip, out);
		for (k=0; k<SW; k++) {
			Y[SW*j+k] = out[k];
		}
	}
}
