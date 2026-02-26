# VL53L0X 거리 측정 펌웨어

STMicroelectronics VL53L0X ToF(Time-of-Flight) 거리 센서를 이용해 측정 거리를 시리얼 및 4자리 7세그먼트 디스플레이에 출력하는 Zephyr RTOS 기반 임베디드 펌웨어입니다.

---

## 하드웨어 구성

| 항목 | 사양 |
|------|------|
| 메인 보드 | STM32 NUCLEO-F401RE |
| 쉴드 | X-NUCLEO-53L0A1 |
| 센서 | VL53L0X (I2C, 주소 `0x29`) |
| 디스플레이 | 4자리 × 7세그먼트 (GPIO 익스팬더 2개) |
| RTOS | Zephyr RTOS |

---

## 기능

- **거리 측정**: VL53L0X ToF 센서로 500ms 주기마다 거리 측정
- **시리얼 출력**: `Distance: X.XXXXXX m` 형식으로 UART 출력
- **디스플레이 출력**: 측정 거리(mm 단위)를 4자리 7세그먼트 디스플레이에 표시
- **오류 표시**: 센서 오류 또는 표시 범위 초과 시 `Err` 표시

---

## 프로젝트 구조

```
vl53/
├── CMakeLists.txt              # Zephyr 빌드 설정
├── prj.conf                    # Kconfig (SENSOR, GPIO 활성화)
├── boards/
│   └── nucleo_f401re.overlay   # NUCLEO-F401RE Device Tree 오버레이
└── src/
    ├── main.c                  # 메인 루프 (센서 측정 및 출력)
    ├── display_7seg.c          # 7세그먼트 디스플레이 드라이버
    └── display_7seg.h          # 디스플레이 상수 및 함수 선언
```

---

## 동작 원리

### 메인 루프 (`main.c`)

```
초기화 → VL53L0X 장치 준비 확인
  ↓
루프 (500ms 주기)
  ├─ sensor_sample_fetch()         // 센서 샘플링
  ├─ sensor_channel_get(DISTANCE)  // 거리 값 읽기
  ├─ printk("Distance: X.XX m")   // 시리얼 출력
  └─ display_number(mm 값)         // 7세그먼트 출력
```

`sensor_value` 구조체에서 `val1`은 정수 미터, `val2`는 소수점 이하 마이크로미터(μm) 단위입니다.
디스플레이 출력 시 `val2 / 100` 연산으로 μm → 0.1mm 단위로 변환합니다.

### 7세그먼트 드라이버 (`display_7seg.c`)

- GPIO 익스팬더 2개(`expander1`, `expander2`)를 통해 각 14핀 제어
- **Common Anode** 방식: 출력값을 반전(XOR `0x3fff`)하여 LOW 신호로 LED 점등
- 4자리 × 7비트 = 28비트 데이터를 두 익스팬더에 분산 전송

**세그먼트 비트 배치:**
```
    3
   ---
 1|   |5
   -2-
 0|   |6
   ---
    4
```

**`display_number(num, base)` 지원 범위 (base=10 기준):**

| 조건 | 표시 범위 |
|------|-----------|
| 양수 | 0 ~ 9999 |
| 음수 | -999 ~ -1 |
| 범위 초과 | `Err` 표시 |

---

## 빌드 및 플래시

### 사전 요구 사항

- [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) 설치
- `west` 툴 설치

### 빌드

```bash
west build -b nucleo_f401re -- -DSHIELD=x_nucleo_53l0a1
```

### 플래시

```bash
west flash
```

### 시리얼 모니터

```bash
west espressif monitor
# 또는
minicom -D /dev/ttyACM0 -b 115200
```

---

## 시리얼 출력 예시

```
VL53L0X ready: vl53l0x@29
Distance: 0.235000 m
Distance: 0.412000 m
Distance: 1.003000 m
```

---

## 참고 자료

- [VL53L0X 데이터시트 (STMicroelectronics)](https://www.st.com/en/imaging-and-photonics-solutions/vl53l0x.html)
- [X-NUCLEO-53L0A1 제품 페이지](https://www.st.com/en/evaluation-tools/x-nucleo-53l0a1.html)
- [Zephyr VL53L0X 드라이버 문서](https://docs.zephyrproject.org/latest/build/dts/api/bindings/sensor/st,vl53l0x.html)

---

## 라이선스

Apache License 2.0
