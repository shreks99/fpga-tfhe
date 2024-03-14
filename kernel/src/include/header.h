// Author: Guanglin Xu (guanglix@andrew.cmu.edu)
//         Carnegie Mellon University
// Date: 05/20/2017
// Copyright reserved.
// Distribution and commercial uses are not allowed without permission.

#ifndef _HEADER_H
#define _HEADER_H

// NPEASE is the problem size.
//  NPEASE can be power of 2. The tested range are 64-4096
//  NPEASE<64 may fail because of pipeline hazards
// Ri is the radix
// SW is the streaming width. SW=n*Ri (n>=1) must be satisfied
#define NPEASE 2048
#define Ri 2
#define SW 2
// must be 2

#define LOGrN 11
#define LOG2N 11
#define LOG2R 1
#define LOG2SW 1
#define LOG2_LOGrN 4

#include "stride_perm_num_stage.h"
#include "dig_rev_perm_num_stage.h"

template <int NUM_STAGE>
struct perm_config{
	int init_perm_idx[SW];

	int w_switch_connection_idx[NUM_STAGE][SW];
	int w_switch_control_bit[NUM_STAGE];
	int w_addr_bit_seq[LOG2N-LOG2SW];

	int r_switch_connection_idx[NUM_STAGE][SW];
	int r_switch_control_bit[NUM_STAGE];
};


#define USE_FFTW
#define DOUBLE
#define FIXED_POINT_PREC 32

#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)


#ifdef SINGLE
typedef float VAL_TYPE;
#endif
#ifdef DOUBLE
typedef double VAL_TYPE;
#endif
#ifdef FIXED_POINT
#include "ap_fixed.h"
typedef ap_fixed<FIXED_POINT_PREC,4> VAL_TYPE;
#endif

typedef struct {
	float real;
	float imag;
} float_cmplx_type;

typedef struct {
	VAL_TYPE real;
	VAL_TYPE imag;
} cmplx_type;

#include "ap_int.h"

typedef struct {
	cmplx_type data;
	ap_uint<LOG2N-LOG2SW+1> addr;
} content_addr;

#include "hls_stream.h"
void disp_complex_type(char* name,  cmplx_type x[NPEASE]);
extern "C" void pease_fft(cmplx_type X[NPEASE], cmplx_type Y[NPEASE]);
void digit_rev(unsigned int in, unsigned int* out, unsigned int bitwid);

void buf_read_addr_generation(ap_uint<LOG2N-LOG2SW> out_count, bool flip, ap_uint<LOG2N-LOG2SW+1> ridx[2]);
void buf_read(cmplx_type buf[SW][NPEASE*2/SW], cmplx_type in_from_buff_pre_switch[SW], ap_uint<LOG2N-LOG2SW+1> ridx_post_switch[SW]);
void buf_write_addr_generation(ap_uint<LOG2N-LOG2SW> in_count, bool flip, int bit_seq[LOG2N-LOG2SW], ap_uint<LOG2N-LOG2SW+1> widx[2]);
void combine_addr_data(content_addr combination[SW], ap_uint<LOG2N-LOG2SW+1> addr[SW],
		cmplx_type data[SW]);
template <typename T>
void switch_network_write(T X[SW], T Y[SW], ap_uint<LOG2N-LOG2SW> bits, int init_perm_idx[SW], int idx_in_or_out[STRIDE_PERM_SWITCH_NUM_STAGE][SW], int control_bit[STRIDE_PERM_SWITCH_NUM_STAGE]);
template <typename T>
void switch_network_read(T X[SW], T Y[SW], ap_uint<LOG2N-LOG2SW> bits, int idx_in_or_out[STRIDE_PERM_SWITCH_NUM_STAGE][SW], int control_bit[STRIDE_PERM_SWITCH_NUM_STAGE]);
void buf_write(cmplx_type buf[SW][NPEASE*2/SW], content_addr in_post_switch[SW]);

#define CMPXADD(Z, X, Y)  ({ \
	(Z).real = (X).real + (Y).real;   \
	(Z).imag = (X).imag + (Y).imag; \
	})
#define CMPXSUB(Z, X, Y)  ({ \
	(Z).real = (X).real - (Y).real; \
	(Z).imag = (X).imag - (Y).imag; \
	})
#define CMPXMUL(Z, X, Y)  ({ \
	(Z).real = (X).real * (Y).real - (X).imag * (Y).imag; \
	(Z).imag = (X).real * (Y).imag + (X).imag * (Y).real; \
	})
#define CMPXSUB_MUL_NI(Z, X, Y)  ({ \
	(Z).imag = -((X).real - (Y).real); \
	(Z).real = (X).imag - (Y).imag; \
	})
#define CMPXSUB_MUL_07R_N07I(Z, X, Y, T)  ({ \
		(T).real = (X).real - (Y).real; \
		(T).imag = (X).imag - (Y).imag; \
		(Z).real = (FP_TYPE)0.70710678118654757 * ((T).real + (T).imag); \
		(Z).imag = (FP_TYPE)0.70710678118654757 * ((T).imag - (T).real); \
	})
#define CMPXSUB_MUL_N07R_N07I(Z, X, Y, T)  ({ \
		(T).real = (X).real - (Y).real; \
		(T).imag = (X).imag - (Y).imag; \
		(Z).real = (FP_TYPE)0.70710678118654757 * ((T).imag - (T).real); \
		(Z).imag = (FP_TYPE)-0.70710678118654757 * ((T).real + (T).imag); \
	})



// 2 sub
#define CMPX_NI_MUL_X_SUB_Y(Z, X, Y)  ({ \
	(Z).real = (X).imag - (Y).imag; \
	(Z).imag = -((X).real - (Y).real); \
	})
// 2 add
#define CMPX_NI_MUL_X_ADD_Y(Z, X, Y)  ({ \
	(Z).real = (X).imag + (Y).imag; \
	(Z).imag = -((X).real + (Y).real); \
	})
// 2 sub
#define CMPX_I_MUL_X_SUB_Y(Z, X, Y)  ({ \
	(Z).real = -((X).imag - (Y).imag); \
	(Z).imag = (X).real - (Y).real; \
	})
// 2 add
#define CMPX_I_MUL_X_ADD_Y(Z, X, Y)  ({ \
	(Z).real = -((X).imag + (Y).imag); \
	(Z).imag = (X).real + (Y).real; \
	})

// 1 add, 5 sub, 2 mul
#define CMPX_TWR_TWR_MUL_X_SUB_Y(Z, TW_R, X, Y )  ({ \
	(Z).real = (FP_TYPE)TW_R * (((X).real - (Y).real) - ((X).imag - (Y).imag)); \
	(Z).imag = (FP_TYPE)TW_R * (((X).real - (Y).real) + ((X).imag - (Y).imag)); \
	})
// 5 add, 1 sub, 2 mul
#define CMPX_TWR_TWR_MUL_X_ADD_Y(Z, TW_R, X, Y)  ({ \
	(Z).real = (FP_TYPE)TW_R * (((X).real + (Y).real) - ((X).imag + (Y).imag)); \
	(Z).imag = (FP_TYPE)TW_R * (((X).real + (Y).real) + ((X).imag + (Y).imag)); \
	})
// 1 add, 5 sub, 2 mul
#define CMPX_TWR_NTWR_MUL_X_SUB_Y(Z, TW_R, X, Y)  ({ \
	(Z).real = (FP_TYPE)TW_R * (((X).real - (Y).real) + ((X).imag - (Y).imag)); \
	(Z).imag = (FP_TYPE)TW_R * (((X).imag - (Y).imag) - ((X).real - (Y).real)); \
	})
// 5 add, 1 sub, 2 mul
#define CMPX_TWR_NTWR_MUL_X_ADD_Y(Z, TW_R, X, Y)  ({ \
	(Z).real = (FP_TYPE)TW_R * (((X).real + (Y).real) + ((X).imag + (Y).imag)); \
	(Z).imag = (FP_TYPE)TW_R * (((X).imag + (Y).imag) - ((X).real + (Y).real)); \
	})

// 1 add, 5 sub, 4 mul
#define CMPX_TWR_TWI_MUL_X_SUB_Y(Z, TW_R, TW_I, X, Y)  ({ \
	(Z).real = (FP_TYPE)TW_R * ((X).real - (Y).real) - TW_I * ((X).imag - (Y).imag); \
	(Z).imag = (FP_TYPE)TW_R * ((X).imag - (Y).imag) + TW_I * ((X).real - (Y).real); \
	})
// 5 add, 1 sub, 4 mul
#define CMPX_TWR_TWI_MUL_X_ADD_Y(Z, TW_R, TW_I, X, Y)  ({ \
	(Z).real = (FP_TYPE)TW_R * ((X).real + (Y).real) - TW_I * ((X).imag + (Y).imag); \
	(Z).imag = (FP_TYPE)TW_R * ((X).imag + (Y).imag) + TW_I * ((X).real + (Y).real); \
	})


#define MAX(X,Y) ({ \
		(X>Y)?(X):(Y); \
	})

#endif
