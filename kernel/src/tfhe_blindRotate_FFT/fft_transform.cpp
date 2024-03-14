/*
 * Free FFT and convolution (C++)
 *
 * Copyright (c) 2021 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/free-small-fft-in-multiple-languages
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string.h>
#include <math.h>
#include <algorithm>
#include "fpga_constants.h"

template<size_t SIZE, size_t WINDOW>
void fft_mult(double *real_out, double *imag_out, const double *real_in, const double *imag_in) {
	const size_t halfsize = WINDOW / 2;
	const size_t tablestep = 2 * N / WINDOW;

	fft_mult_window: for (size_t i = 0; i < SIZE; i += WINDOW) {
		fft_mult_mult: for (size_t j = 0; j < halfsize; j++) {
			#pragma HLS pipeline II=1
			const size_t first = j + i;
			const size_t second = j + i + halfsize;
			const int k = j * tablestep;
			// Load
			double tprel = real_in[second];
			double tpiml = imag_in[second];
			double tprej = real_in[first];
			double tpimj = imag_in[first];
			double tpcos = cosTable[k];
			double tpsin = sinTable[k];
			// Calc
			double calcre =  tprel * tpcos + tpiml * tpsin;
			double calcim = -tprel * tpsin + tpiml * tpcos;
			// Store
			real_out[second] = tprej - calcre;
			imag_out[second] = tpimj - calcim;
			real_out[first] = tprej + calcre;
			imag_out[first] = tpimj + calcim;
		}
	}
}

template <size_t SIZE, size_t WINDOW>
void fft_mult_temp(double *real_out, double *imag_out, const double *real_in, const double *imag_in) {
	constexpr size_t halfsize = WINDOW / 2;
	constexpr size_t tablestep = 2 * N / WINDOW;

	fft_mult_window_template: for (size_t i = 0; i < SIZE; i += WINDOW) {
		fft_mult_mult_template: for (size_t j = 0; j < halfsize; j++) {
			#pragma HLS pipeline II=1
			const size_t first = j + i;
			const size_t second = j + i + halfsize;
			const int k = j * tablestep;
			// Load
			double tprel = real_in[second];
			double tpiml = imag_in[second];
			double tprej = real_in[first];
			double tpimj = imag_in[first];
			double tpcos = cosTable[k];
			double tpsin = sinTable[k];
			// Calc
			double calcre =  tprel * tpcos + tpiml * tpsin;
			double calcim = -tprel * tpsin + tpiml * tpcos;
			// Store
			real_out[second] = tprej - calcre;
			imag_out[second] = tpimj - calcim;
			real_out[first] = tprej + calcre;
			imag_out[first] = tpimj + calcim;
		}
	}
}

template<size_t SIZE, size_t WINDOW>
void fft_mult_parallel(double real_out[4][2 * N/4], double imag_out[4][2 * N/4], const double real_in[4][2 * N/4], const double imag_in[4][2 * N/4]) {
	fft_mult_parallel: for(int i=0; i<4; i++) {
		#pragma HLS unroll
		fft_mult_temp<SIZE/4, WINDOW>(real_out[i], imag_out[i], real_in[i], imag_in[i]);
	}
}

// extern "C" {
	void fft_transform(double *real, double *imag) {
		double re1[4][2 * N/4];
		double im1[4][2 * N/4];
		double re2[4][2 * N/4];
		double im2[4][2 * N/4];

		#pragma HLS array_partition variable=re1 complete dim=1
		#pragma HLS array_partition variable=im1 complete dim=1
		#pragma HLS array_partition variable=re2 complete dim=1
		#pragma HLS array_partition variable=im2 complete dim=1

		// Bit reversal
		fft_bit_reverse_1: for(int i=0; i<4; i++) {
			fft_bit_reverse_2: for(int j=0; j<2 * N/4; j++) {
				uint64_t br = bit_reversed[(2 * N/4)*i + j];
				re1[i][j] = real[br];
				im1[i][j] = imag[br];
			}
		}

		// At this point, temp arrays hold the bit-reversed elements
		// Now we swap between the 2 memories to perform radix2fft
		// To do full FFT we need to do fft_mult for all powers of 2 from 2 to 2N.
		fft_mult_parallel<2048, 2>(re2, im2, re1, im1); // (re2,im2) = radix2fft(re1, im1)
		fft_mult_parallel<2048, 4>(re1, im1, re2, im2); // (re1,im1) = radix2fft(re2, im2)
		fft_mult_parallel<2048, 8>(re2, im2, re1, im1); // ...
		fft_mult_parallel<2048, 16>(re1, im1, re2, im2);
		fft_mult_parallel<2048, 32>(re2, im2, re1, im1);
		fft_mult_parallel<2048, 64>(re1, im1, re2, im2); // 32 parallel
		fft_mult_parallel<2048, 128>(re2, im2, re1, im1); // 16 parallel
		fft_mult_parallel<2048, 256>(re1, im1, re2, im2); // 8 parallel
		fft_mult_parallel<2048, 512>(re2, im2, re1, im1); // 4 parallel

		for(int i=0; i<4; i++) {
			for(int j=0; j<2 * N/4; j++) {
				real[(2 * N/4)*i + j] = re2[i][j];
				imag[(2 * N/4)*i + j] = im2[i][j];
			}
		}

		double real_temp[2 * N];
		double imag_temp[2 * N];

		fft_mult<2048, 1024>(real_temp, imag_temp, real, imag); // 2 parallel
		fft_mult<2048, 2048>(real, imag, real_temp, imag_temp); // Final result stored in real, imag
	}
// }
