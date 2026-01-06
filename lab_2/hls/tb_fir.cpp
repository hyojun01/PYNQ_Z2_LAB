#include <iostream>
#include <fstream>
#include <cmath>
#include "fir.h"

using namespace std;

// 시뮬레이션 설정
const int NUM_SAMPLES = 600;  // 0.6초 분량 (fs=100 MHz 가정)
const double FS = 100000000.0;
const int ADC_MAX = 32767;
const int ADC_MIN = -32768;

int main() {
    // 1. 스트림 선언
    hls::stream<axis_t> in_stream;
    hls::stream<axis_t> out_stream;
    
    axis_t val_in, val_out;

    cout << ">> Start FIR Filter Simulation" << endl;

    // 2. 입력 데이터 생성 및 스트림 주입 (Stimulus Generation)
    // Python 코드와 동일: 5 MHz 신호 + 15 MHz 노이즈
    for (int i = 0; i < NUM_SAMPLES; i++) {
        double t = i / FS;
        
        // 입력 신호 생성 (실수형)
        double sig_clean = 0.5 * std::sin(2 * M_PI * 5000000 * t);
        double sig_noise = 0.25 * std::sin(2 * M_PI * 15000000 * t);
        double sig_input = sig_clean + sig_noise;

        // ADC 변환 (Rounding & Saturation)
        int raw_int = (int)round(sig_input * 32768.0);
        if (raw_int > ADC_MAX) raw_int = ADC_MAX;
        if (raw_int < ADC_MIN) raw_int = ADC_MIN;

        short adc_code = (short)raw_int;

        // AXI Stream 패킷 구성
        val_in.data = (data_t)((double)adc_code / 32768.0); // 실수 -> Fixed Point 변환
        val_in.keep = -1; // 모든 바이트 유효 (All ones)
        val_in.strb = -1;
        
        // 마지막 샘플에서 TLAST 신호를 1로 설정 (매우 중요!)
        // DUT 내부의 while(1) 루프를 탈출시키는 트리거입니다.
        val_in.last = (i == NUM_SAMPLES - 1) ? 1 : 0;

        in_stream.write(val_in);
    }

    // 3. FIR 필터 IP 실행 (Device Under Test)
    fir(in_stream, out_stream);

    // 4. 출력 데이터 읽기 및 저장
    cout << ">> Processing Output Stream..." << endl;
    
    // 입력 개수만큼 출력을 읽어옴
    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (out_stream.empty()) {
            cerr << "Error: Output stream is empty at index " << i << endl;
            break;
        }
        
        out_stream.read(val_out);
    }

    cout << ">> Simulation Finished." << endl;

    return 0;
}