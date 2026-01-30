# PYNQ_Z2_LAB

> Example of Digital Signal Processing using PYNQ on Xilinx PYNQ-Z2 Board.

PYNQ-Z2 보드에서 Vitis HLS와 Vivado를 활용하여 다양한 하드웨어 IP를 설계하고, Jupyter Notebook 환경에서 검증한 실습 내용을 기록한다.

---

## 📁 디렉토리 구조

```
PYNQ_Z2_LAB/
├── lab_1/                    # Adder
│   ├── hls/                  # Vitis HLS 소스코드
│   ├── pynq/                 # PYNQ Overlay 및 Jupyter Notebook
│   └── vivado/               # Vivado 프로젝트
├── lab_2/                    # FIR Filter
│   ├── hls/                  # Vitis HLS 소스코드
│   ├── pynq/                 # PYNQ Overlay 및 Jupyter Notebook
│   ├── python/               # Python 유틸리티 스크립트
│   └── vivado/               # Vivado 프로젝트
└── lab_3/                    # FFT
    ├── hls/                  # Vitis HLS 소스코드
    ├── pynq/                 # PYNQ Overlay 및 Jupyter Notebook
    ├── python/               # Python 유틸리티 스크립트
    └── vivado/               # Vivado 프로젝트
```

---

## 🧪 LAB 01: Adder

### 개요
PYNQ에서 IP의 레지스터 제어를 통한 간단한 산술 연산 실습

### 설명
간단한 산술 연산(`Y = A + 2*B - C`)을 수행하는 HLS IP를 제작하고, 이를 PYNQ Overlay로 비트스트림을 올려 Python 환경에서 제어한다.

- **인터페이스**: AXI-Lite (S_AXILITE)
- **연산**: `Y = A + 2*B - C`
- **레지스터 맵**:
  - `0x10`: A 입력
  - `0x18`: B 입력
  - `0x20`: C 입력
  - `0x28`: Y 출력

### 핵심 학습 내용
- PYNQ Overlay를 통한 비트스트림 로드
- IP 레지스터 직접 읽기/쓰기 (`read()`, `write()`)
- HLS IP 제어 신호 (ap_start, ap_done) 처리

---

## 🧪 LAB 02: FIR Filter

### 개요
PYNQ에서 시뮬레이트된 신호를 AXI-DMA를 이용하여 IP에 스트리밍, IP에서 FIR 신호 처리를 거치고 이를 PYNQ에서 확인

### 설명
45-tap 대칭 FIR 저역통과 필터(Low-Pass Filter)를 HLS로 구현하고, AXI-DMA를 통해 PYNQ와 데이터를 스트리밍 방식으로 주고받는다.

- **인터페이스**: AXI-Stream (입력/출력), AXI-Lite (제어)
- **필터 사양**: 45-tap 대칭 FIR 필터
- **테스트 신호**: 4MHz + 15MHz 합성 신호 (100MHz 샘플링)
- **DMA 버퍼**: `pynq.allocate()`를 통한 연속 메모리 할당

### 핵심 학습 내용
- AXI-DMA를 통한 PS-PL 간 데이터 스트리밍
- 연속 메모리 버퍼 할당 (`allocate`)
- 소프트웨어 FIR vs 하드웨어 FIR 성능 비교
- 실시간 신호 처리 결과 시각화

---

## 🧪 LAB 03: FFT (Fast Fourier Transform)

### 개요
PYNQ에서 시뮬레이트된 신호를 AXI-DMA를 이용하여 IP에 스트리밍, IP에서 FFT 신호 처리를 거치고 이를 PYNQ에서 확인

### 설명
FFT(Fast Fourier Transform) 알고리즘을 HLS로 구현하고, AXI-DMA를 통해 시간 영역 신호를 주파수 영역으로 변환한다.

- **인터페이스**: AXI-Stream (입력/출력), AXI-Lite (제어)
- **FFT 크기**: N-point FFT (Radix-2 Cooley-Tukey 알고리즘)
- **데이터 타입**: 복소수 (실수부 + 허수부)
- **최적화**: Bit-reversal, Dataflow 파이프라인

### 핵심 학습 내용
- Cooley-Tukey FFT 알고리즘 이해
- 복소수 데이터 스트리밍 처리
- HLS DATAFLOW 프라그마를 통한 파이프라인 최적화
- 주파수 스펙트럼 분석 및 시각화

---

## 🛠️ 개발 환경

- **Board**: Xilinx PYNQ-Z2
- **HLS Tool**: Vitis HLS
- **FPGA Tool**: Vivado Design Suite
- **Software**: PYNQ Framework, Jupyter Notebook, Python 3