#include "adder.h"

void adder(data_t A, data_t B, data_t C, data_t &Y){
#pragma HLS INTERFACE mode = s_axilite port = A bundle = CONTROL
#pragma HLS INTERFACE mode = s_axilite port = B bundle = CONTROL
#pragma HLS INTERFACE mode = s_axilite port = C bundle = CONTROL
#pragma HLS INTERFACE mode = s_axilite port = Y bundle = CONTROL 
#pragma HLS INTERFACE mode = s_axilite port = return bundle = CONTROL

    Y = A + 2 * B - C;
}