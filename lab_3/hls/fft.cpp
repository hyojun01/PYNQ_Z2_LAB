#include "fft.h"

void stream_to_buffer(hls::stream<axis_t> &stream, data_t buffer_R[FFT_SIZE], data_t buffer_I[FFT_SIZE]) {

    loop_stream_to_buffer: 
    for (int i = 0; i < FFT_SIZE; i++) {
        axis_t temp;
        stream.read(temp);
        buffer_R[i] = temp.data.real();
        buffer_I[i] = temp.data.imag();
    }
}

void buffer_to_stream(data_t buffer_R[FFT_SIZE], data_t buffer_I[FFT_SIZE], hls::stream<axis_t> &stream) {

    static int count = 1;

    loop_buffer_to_stream:
    for (int i = 0; i < FFT_SIZE; i++) {
        axis_t temp;
        temp.data = std::complex<float>(buffer_R[i], buffer_I[i]);
        if (count == 10 && i == FFT_SIZE - 1) {
            temp.last = 1;
            count = 1;
        }
        else {
            temp.last = 0;
        }
        stream.write(temp);
    }
    count++;
}

unsigned int reverse_bits(unsigned int input) {
    unsigned int rev = 0;
    unsigned int i = 0;
    loop_reverse_bits:
    for (i = 0; i < EXP2_FFT; i++) {
        rev = (rev << 1) | (input & 1);
        input = input >> 1;
    }
    return rev;
}

void bit_reverse(data_t in_R[FFT_SIZE], data_t in_I[FFT_SIZE], data_t out_R[FFT_SIZE], data_t out_I[FFT_SIZE]) {
    unsigned int reversed;
    unsigned int i;

    loop_bit_reverse:
    for (i = 0; i < FFT_SIZE; i++) {
    #pragma HLS PIPELINE II = 3
        reversed = reverse_bits(i);
        if (i <= reversed) {
            //swap real part
            out_R[reversed] = in_R[i];
            out_R[i] = in_R[reversed];

            //swap imag part
            out_I[reversed] = in_I[i];
            out_I[i] = in_I[reversed];
        }
    }
}

void fft_stage(int stage, data_t in_R[FFT_SIZE], data_t in_I[FFT_SIZE], data_t out_R[FFT_SIZE], data_t out_I[FFT_SIZE], data_t tw_R[FFT_SIZE2], data_t tw_I[FFT_SIZE2]) {
    int DFTpts = 1 << stage;
    int numBF = DFTpts / 2;

    loop_group_level:
    for (int j = 0; j < numBF; j++) {
        loop_pair_level:
        for (int i = j; i < FFT_SIZE; i += DFTpts) {
            int i_lower = i + numBF;
            //complex multiplication
            data_t temp_R = in_R[i_lower] * tw_R[j] - in_I[i_lower] * tw_I[j];
            data_t temp_I = in_I[i_lower] * tw_R[j] + in_R[i_lower] * tw_I[j];
            //comple addition
            out_R[i] = in_R[i] + temp_R;
            out_I[i] = in_I[i] + temp_I;
            out_R[i_lower] = in_R[i] - temp_R;
            out_I[i_lower] = in_R[i] - temp_I;
        }
    }

}

void fft_top(hls::stream<axis_t> &input, hls::stream<axis_t> &output) {
#pragma HLS INTERFACE mode = axis port = input 
#pragma HLS INTERFACE mode = axis port = output
#pragma HLS INTERFACE mode = s_axilite port = return
#pragma HLS DATAFLOW

    data_t input_R[FFT_SIZE];
    data_t input_I[FFT_SIZE];
    data_t stage_R[EXP2_FFT][FFT_SIZE];
    data_t stage_I[EXP2_FFT][FFT_SIZE];
    data_t output_R[FFT_SIZE];
    data_t output_I[FFT_SIZE];
    data_t twiddles_R[FFT_SIZE2];
    data_t twiddles_I[FFT_SIZE2];
    #pragma HLS ARRAY_PARTITION variable = stage_R type = complete dim = 1
    #pragma HLS ARRAY_PARTITION variable = stage_I type = complete dim = 1
    #pragma HLS ARRAY_PARTITION variable = twiddles_R type = complete dim = 1
    #pragma HLS ARRAY_PARTITION variable = twiddles_I type = complete dim = 1

    loop_twiddles:
    for (int i = 0; i < FFT_SIZE2; i++) {
        data_t angle = -2 * PI * i / FFT_SIZE;
        data_t tf_R = (data_t)cos(angle);
        data_t tf_I = (data_t)sin(angle);
        twiddles_R[i] = tf_R;
        twiddles_I[i] = tf_I;
    }

    stream_to_buffer(input, input_R, input_I);
    bit_reverse(input_R, input_I, stage_R[0], stage_I[0]);
    for (int stage = 1; stage < EXP2_FFT; stage++) {
    #pragma HLS UNROLL
        fft_stage(stage, stage_R[stage - 1], stage_I[stage - 1], stage_R[stage], stage_I[stage], twiddles_R, twiddles_I);
    }
    fft_stage(EXP2_FFT, stage_R[EXP2_FFT - 1], stage_I[EXP2_FFT - 1], output_R, output_I, twiddles_R, twiddles_I);
    buffer_to_stream(output_R, output_I, output);

}