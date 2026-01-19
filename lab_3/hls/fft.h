#ifndef __FFT_H__
#define __FFT_H__

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include <cmath>
#include <complex>

#define FFT_SIZE 256
#define FFT_SIZE2 128
#define EXP2_FFT 8

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef float data_t;
typedef hls::axis<std::complex<float>> axis_t;

void fft_top(hls::stream<axis_t> &input, hls::stream<axis_t> &output);

#endif