#include "fir.h"

void fir(hls::stream<axis_t> &input, hls::stream<axis_t> &output) {
#pragma HLS INTERFACE mode = axis port = input
#pragma HLS INTERFACE mode = axis port = output
#pragma HLS INTERFACE mode = s_axilite port = return bundle = CONTROL

    axis_t tmp;
    static const coef_t coefficient[HALF_TAPS] = {
        -0.00875854, 0.04141235, 0.01757812, 0.00823975, -0.00152588, -0.01113892,
        -0.01669312, -0.01473999, -0.00497437, 0.00930786, 0.02175903, 0.02578735,
        0.01751709, -0.00210571, -0.02609253, -0.04367065, -0.04370117, -0.01934814,
        0.0289917, 0.09240723,  0.15591431,  0.20275879,  0.22000122
    };
    static data_t shift_register[TAPS] = {0};
    #pragma HLS ARRAY_PARTITION variable = coefficient type = complete dim = 0
    #pragma HLS ARRAY_PARTITION variable = shift_register type = complete dim = 0

    while (1) {
    #pragma HLS PIPELINE II = 1
        input.read(tmp);
        shift_register_loop:
        for (int i = TAPS - 1; i > 0; i--) {
        #pragma HLS UNROLL
            shift_register[i] = shift_register[i - 1];
        }
        shift_register[0] = tmp.data;
        acc_t sum = 0;
        convolution_loop:
        for (int i = 0; i < HALF_TAPS - 1; i++) {
        #pragma HLS UNROLL
            sum += (shift_register[i] + shift_register[TAPS - 1 - i]) * coefficient[i];
        }
        sum += shift_register[HALF_TAPS - 1] * coefficient[HALF_TAPS - 1];
        tmp.data = (data_t)sum;
        output.write(tmp);
        if (tmp.last) {
            break;
        }
    }
}