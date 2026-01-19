import pandas as pd
import matplotlib.pyplot as plt

# ---------------------------------------------------------
# 설정 파라미터
# ---------------------------------------------------------
FILENAME = 'fft_result.csv'
FFT_SIZE = 512
NUM_FRAMES = 10  # 예상되는 프레임 수

def plot_continuous_fft():
    try:
        # 1. CSV 파일 불러오기
        df = pd.read_csv(FILENAME)
    except FileNotFoundError:
        print(f"오류: '{FILENAME}' 파일을 찾을 수 없습니다.")
        return

    # 데이터가 비어있는지 확인
    if df.empty:
        print("데이터가 비어있습니다.")
        return

    # 2. 그래프 그리기
    plt.figure(figsize=(12, 6))

    # 전체 Magnitude 데이터를 1차원 배열로 간주하고 이어 그리기
    # x축: 전체 샘플 인덱스 (0 ~ 1279), y축: Magnitude
    plt.plot(df.index, df['Magnitude'], label='FFT Output Stream', color='b', linewidth=1)

    # 3. 프레임 구분선 추가 (128 샘플마다)
    # 각 프레임이 어디서 시작하고 끝나는지 시각적으로 구분
    for i in range(1, NUM_FRAMES + 1):
        x_pos = i * FFT_SIZE
        plt.axvline(x=x_pos, color='red', linestyle='--', alpha=0.5)
        # 구분선 위에 프레임 번호 표시 (선택사항)
        if i < NUM_FRAMES:
             plt.text(x_pos - (FFT_SIZE/2), df['Magnitude'].max()*0.9, 
                      f'Frame {i-1}', ha='center', color='red', fontsize=9)
             
    plt.text(FFT_SIZE/2, df['Magnitude'].max()*0.9, 
             'Frame 0', ha='center', color='red', fontsize=9)

    # 그래프 꾸미기
    plt.title('Continuous FFT Output Stream (All Frames Stitched)', fontsize=14)
    plt.xlabel('Global Sample Index (Frame No * 128 + Bin Index)', fontsize=12)
    plt.ylabel('Magnitude', fontsize=12)
    plt.grid(True, which='both', linestyle='-', alpha=0.3)
    plt.legend(loc='upper right')
    
    # 꽉 차게 보여주기
    plt.xlim(0, len(df))
    plt.tight_layout()
    
    plt.show()

if __name__ == "__main__":
    plot_continuous_fft()