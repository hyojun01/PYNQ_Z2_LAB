import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

def to_fixed_point(coeffs, total_bits, int_bits):
    """
    실수형 계수를 Vitis HLS ap_fixed<total_bits, int_bits> 스타일로 양자화하는 함수
    """
    frac_bits = total_bits - int_bits
    scale = 2.0 ** frac_bits
    
    # 1. 스케일링 및 반올림 (Quantization)
    coeffs_fixed = np.round(coeffs * scale)
    
    # 2. 범위 제한 (Saturation / Clipping)
    # 2의 보수 표현 범위: -2^(N-1) ~ 2^(N-1)-1
    max_val = (2 ** (total_bits - 1)) - 1
    min_val = -(2 ** (total_bits - 1))
    
    coeffs_fixed = np.clip(coeffs_fixed, min_val, max_val)
    
    # 3. 시뮬레이션을 위해 다시 실수 스케일로 복원
    # (실제 FPGA에서는 정수로 저장되지만, Python 그래프 비교를 위해)
    return coeffs_fixed / scale

def compare_filters():
    # ---------------------------------------------------------
    # 1. 필터 설계 (Parks-McClellan)
    # ---------------------------------------------------------
    fs = 100000000.0          # 샘플링 주파수(100 MHz)
    pass_cutoff = 10000000.0   # 통과 대역 끝 (10 MHz)
    trans_width = 2000000.0   # 천이 대역 폭 (2 MHz)
    stop_cutoff = pass_cutoff + trans_width # 저지 대역 시작 (12 MHz)

    numtaps = 45  # 탭 수
    
    bands = [0, pass_cutoff, stop_cutoff, 0.5 * fs]
    desired = [1, 0]
    
    # 실수형(Float64) 오리지널 계수
    b_float = signal.remez(numtaps, bands, desired, fs=fs)
    
    # ---------------------------------------------------------
    # 2. 고정 소수점 변환 (Quantization)
    # ---------------------------------------------------------
    # 예: ap_fixed<16, 1> -> 전체 16비트, 정수 1비트(부호포함), 소수 15비트
    total_bits = 16
    int_bits = 1   # 계수들이 보통 -1 ~ 1 사이이므로 1비트면 충분 (혹은 2)
    
    b_fixed = to_fixed_point(b_float, total_bits, int_bits)
    
    # 양자화 오차 계산
    quantization_error = b_float - b_fixed
    print(f"Max Quantization Error: {np.max(np.abs(quantization_error)):.7f}")

    # ---------------------------------------------------------
    # 3. 주파수 응답 계산
    # ---------------------------------------------------------
    w, h_float = signal.freqz(b_float, [1], worN=4096, fs=fs)
    w, h_fixed = signal.freqz(b_fixed, [1], worN=4096, fs=fs)
    
    # ---------------------------------------------------------
    # 4. 비교 그래프 (Frequency Response)
    # ---------------------------------------------------------
    plt.figure(figsize=(12, 10))
    
    # [상단] 전체 주파수 응답 비교
    plt.subplot(2, 1, 1)
    plt.plot(w, 20 * np.log10(abs(h_float)), 'b', linewidth=1.5, label='Float (Original)')
    plt.plot(w, 20 * np.log10(abs(h_fixed)), 'r--', linewidth=1.5, label=f'Fixed-Point <{total_bits},{int_bits}>')
    plt.title(f'Frequency Response Comparison (Float vs Fixed<{total_bits},{int_bits}>)')
    plt.ylabel('Magnitude (dB)')
    plt.xlabel('Frequency (Hz)')
    plt.grid(True, which='both', alpha=0.5)
    plt.legend()
    plt.ylim(-100, 5) # Y축 범위 제한
    
    # [하단] 저지 대역(Stopband) 확대 - 성능 저하가 가장 잘 보이는 곳
    plt.subplot(2, 1, 2)
    plt.plot(w, 20 * np.log10(abs(h_float)), 'b', label='Float')
    plt.plot(w, 20 * np.log10(abs(h_fixed)), 'r--', label='Fixed-Point')
    plt.title('Stopband Detail (Zoom In)')
    plt.ylabel('Magnitude (dB)')
    plt.xlabel('Frequency (Hz)')
    plt.xlim(stop_cutoff - 10, fs/2) # 저지 대역 부분만 확대
    plt.ylim(-100, -10) # 관심 있는 dB 영역 확대
    plt.grid(True, which='both', alpha=0.5)
    plt.legend()
    
    plt.tight_layout()
    plt.show()

    # 계수 출력
    print(f"Float Filter Coefficients:")
    print(b_float[:])

    print(f"Fixed Filter Coefficients:")
    print(b_fixed[:])

if __name__ == "__main__":
    compare_filters()