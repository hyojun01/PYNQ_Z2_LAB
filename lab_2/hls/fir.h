#ifndef __FIR_H__
#define __FIR_H__

#include "ap_fixed.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

#define TAPS 45
#define HALF_TAPS 23

typedef ap_fixed<16, 1> coef_t;
typedef ap_fixed<16, 1> data_t;
typedef ap_fixed<45, 15> acc_t;
typedef hls::axis<data_t, 0, 0, 0> axis_t;

void fir(hls::stream<axis_t> &input, hls::stream<axis_t> &output);

#endif