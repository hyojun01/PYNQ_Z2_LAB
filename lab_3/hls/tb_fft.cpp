#include "fft.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

// 시뮬레이션 파라미터 정의
#define SAMPLING_RATE 100e6 // 100 MHz
#define FREQ_1        125e5  // 10 MHz
#define FREQ_2        25e6  // 30 MHz
#define AMP_1         0.5   // Magnitude 0.5
#define AMP_2         0.5  // Magnitude 0.25
#define NUM_FRAMES    10    // 128개 샘플 * 10 프레임

using namespace std;

int main() {
    // 1. 스트림 선언
    hls::stream<axis_t> input_stream("input_stream");
    hls::stream<axis_t> output_stream("output_stream");

    // 결과 저장을 위한 파일 열기
    ofstream result_file("fft_result.csv");
    result_file << "Frame,Bin_Index,Real,Imag,Magnitude" << endl;

    cout << ">> Start FFT Simulation..." << endl;
    cout << "   Sampling Rate: " << SAMPLING_RATE / 1e6 << " MHz" << endl;
    cout << "   Target Freq 1: " << FREQ_1 / 1e6 << " MHz (Amp: " << AMP_1 << ")" << endl;
    cout << "   Target Freq 2: " << FREQ_2 / 1e6 << " MHz (Amp: " << AMP_2 << ")" << endl;

    // 2. 프레임 루프 (10번 연속 수행)
    for (int frame = 0; frame < NUM_FRAMES; frame++) {
        cout << ">> Processing Frame " << frame << "..." << endl;

        // ------------------------------------------------------
        // Step A: 입력 신호 생성 및 스트림 입력
        // ------------------------------------------------------
        for (int i = 0; i < FFT_SIZE; i++) {
            // 현재 샘플의 절대 시간(t) 계산
            // 전체 누적 샘플 인덱스 = (frame * FFT_SIZE) + i
            double t = ((double)(frame * FFT_SIZE + i)) / SAMPLING_RATE;

            // 신호 합성
            float val = AMP_1 * sin(2 * PI * FREQ_1 * t) + 
                        AMP_2 * sin(2 * PI * FREQ_2 * t);
            // float val = AMP_2 * sin(2 * PI * FREQ_2 * t);

            axis_t in_data;
            // 실수는 입력값, 허수는 0으로 설정하여 복소수 데이터 구성
            in_data.data = std::complex<float>(val, 0.0f);
            
            // TLAST 신호 처리 (프레임의 마지막 데이터일 때 1)
            in_data.last = (i == FFT_SIZE - 1) ? 1 : 0;
            in_data.keep = -1; // 모든 바이트 유효
            
            input_stream.write(in_data);
        }

        // ------------------------------------------------------
        // Step B: DUT(Design Under Test) 실행
        // ------------------------------------------------------
        fft_top(input_stream, output_stream);

        // ------------------------------------------------------
        // Step C: 결과 읽기 및 검증
        // ------------------------------------------------------
        if (output_stream.empty()) {
            cout << "!! Error: Output stream is empty!" << endl;
            return -1;
        }

        for (int i = 0; i < FFT_SIZE; i++) {
            axis_t out_data;
            output_stream.read(out_data);

            float out_r = out_data.data.real();
            float out_i = out_data.data.imag();
            // Magnitude 계산: sqrt(Real^2 + Imag^2)
            float magnitude = sqrt(out_r * out_r + out_i * out_i);

            // CSV 파일에 저장
            result_file << frame << "," << i << "," 
                        << out_r << "," << out_i << "," 
                        << magnitude << endl;
        }
    }

    result_file.close();
    cout << ">> Simulation Finished." << endl;
    cout << ">> Check 'fft_result.csv' for details." << endl;

    // ------------------------------------------------------
    // 이론적 Bin 위치 계산 (참고용)
    // Bin Index = (Target Freq / Sampling Rate) * FFT_SIZE
    // ------------------------------------------------------
    double bin1 = (FREQ_1 / SAMPLING_RATE) * FFT_SIZE;
    double bin2 = (FREQ_2 / SAMPLING_RATE) * FFT_SIZE;

    return 0;
}