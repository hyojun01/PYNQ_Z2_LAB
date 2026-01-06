import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

def design_and_test_remez_filter():
    # ---------------------------------------------------------
    # 1. 필터 사양 설정
    # ---------------------------------------------------------
    fs = 100000000.0          # 샘플링 주파수(100 MHz)
    pass_cutoff = 10000000.0   # 통과 대역 끝 (10 MHz)
    trans_width = 2000000.0   # 천이 대역 폭 (2 MHz)
    stop_cutoff = pass_cutoff + trans_width # 저지 대역 시작 (12 MHz)
    
    # Remez 알고리즘은 탭 수가 너무 적으면 수렴하지 않을 수 있고,
    # 너무 많으면 연산량이 늘어납니다. 여기서는 45탭을 사용합니다.
    numtaps = 45         
    
    # ---------------------------------------------------------
    # 2. Remez 알고리즘으로 계수 생성
    # ---------------------------------------------------------
    # 대역 정의: [0 ~ 50Hz] 통과, [60Hz ~ 500Hz] 차단
    bands = [0, pass_cutoff, stop_cutoff, 0.5 * fs]
    desired = [1, 0] # 통과 대역 Gain=1, 저지 대역 Gain=0
    
    b = signal.remez(numtaps, bands, desired, fs=fs)
    
    # ---------------------------------------------------------
    # 3. 테스트 신호 생성 (Simulation Stream)
    # ---------------------------------------------------------
    duration = 0.000001 # 1 usec 동안 시뮬레이션
    t = np.arange(0, duration, 1/fs)
    
    # 원하는 신호: 5 MHz (통과 대역 내)
    sig_clean = np.sin(2 * np.pi * 5000000 * t)
    
    # 제거할 노이즈: 15 MHz (저지 대역 내, 12 MHz 이상이므로 깎여야 함)
    sig_noise = 0.5 * np.sin(2 * np.pi * 15000000 * t)
    
    # 입력 신호 합성
    sig_input = sig_clean + sig_noise
    
    # ---------------------------------------------------------
    # 4. 필터링 수행
    # ---------------------------------------------------------
    # FIR 필터 적용 (a=[1.0])
    sig_filtered = signal.lfilter(b, [1.0], sig_input)
    
    # ---------------------------------------------------------
    # 5. 결과 시각화
    # ---------------------------------------------------------
    plt.figure(figsize=(12, 10))
    
    # [그래프 1] 주파수 응답 (Bode Plot)
    w, h = signal.freqz(b, [1], worN=2048, fs=fs)
    plt.subplot(2, 1, 1)
    plt.plot(w, 20 * np.log10(abs(h)), 'b')
    plt.title('Remez FIR Filter Frequency Response')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Gain (dB)')
    plt.grid(True)
    
    # 가이드라인 표시
    plt.axvline(pass_cutoff, color='g', linestyle='--', label='Pass Edge (10 MHz)')
    plt.axvline(stop_cutoff, color='r', linestyle='--', label='Stop Edge (12 MHz)')
    plt.legend()
    plt.ylim(-80, 5) # Y축 범위 고정
    
    # [그래프 2] 시간 영역 필터링 결과
    plt.subplot(2, 1, 2)
    plt.plot(t, sig_input, 'k-', alpha=0.3, label='Input (5 MHz + 15 MHz Noise)')
    plt.plot(t, sig_filtered, 'g-', linewidth=2, label='Filtered Output')
    
    # 위상 지연(Group Delay) 설명
    # FIR 필터(Linear Phase)의 지연 샘플 수 = (N-1) / 2
    delay = 0.5 * (numtaps - 1) / fs
    plt.title(f'Time Domain Signal (Note the Group Delay approx {delay*1000:.1f}ms)')
    
    plt.xlabel('Time (seconds)')
    plt.ylabel('Amplitude')
    plt.legend(loc='upper right')
    plt.grid(True)
    
    plt.tight_layout()
    plt.show()

    # 계수 출력
    print(f"Filter Coefficients:")
    print(b[:])

if __name__ == "__main__":
    design_and_test_remez_filter()