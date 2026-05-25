# Soil Moisture Sensor Test

**Status:** ✅ Complete  
**Location:** `firmware/tests/capacitor_sensor_test/`  
**Sketch:** `capacitor_sensor_test.ino`

---

## Purpose

Validates the capacitive soil moisture sensor by confirming stable ADC readings, establishing calibration constants, and verifying the raw-to-percentage mapping before integration into production firmware.

---

## Hardware

| Component | Detail |
|---|---|
| Microcontroller | ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1) |
| Sensor | Capacitive soil moisture sensor v1.2 |
| Signal pin | GPIO34 (ADC1_CH6 — input only) |
| Power | 3.3V (always-on rail during test) |

**Wiring:**

```
Sensor VCC  →  ESP32 3.3V
Sensor GND  →  ESP32 GND
Sensor AOUT →  ESP32 GPIO34
```

> **Note:** GPIO34 is on ADC1. Do not substitute ADC2 pins (GPIO0, 2, 4, 12–15, 25–27) — ADC2 conflicts with the WiFi radio and will be required in production firmware.

---

## Calibration Results

Calibration performed with sensor held in still air and then fully submerged in tap water. Readings were averaged over 10 samples.

| Condition | Raw ADC Value |
|---|---|
| Dry air (sensor out of soil) | **3225** |
| Submerged in water | **1100** |

These values are set as constants in the sketch and used to map raw ADC output to a 0–100% moisture percentage via Arduino's `map()` function.

```cpp
const int AIR_VALUE   = 3225;
const int WATER_VALUE = 1100;
```

---

## Moisture Mapping

| Moisture % | Status Label |
|---|---|
| 0–19% | DRY |
| 20–39% | LOW |
| 40–59% | MODERATE |
| 60–79% | MOIST |
| 80–100% | WET |

Values outside the calibrated range are clamped by `constrain()` and trigger a serial warning.

---

## Test Procedure

1. Flashed sketch with `CALIBRATION_MODE = true`
2. Held sensor in dry air → recorded raw ADC ≈ **3225**
3. Submerged sensor in water → recorded raw ADC ≈ **1100**
4. Updated `AIR_VALUE` and `WATER_VALUE` constants
5. Re-flashed with `CALIBRATION_MODE = false`
6. Confirmed percentage output and status labels printed correctly over Serial at 115200 baud

---

## Serial Output (Normal Mode)

```
============================================
  Soil Moisture Sensor Test
  ESP32 Solar Monitor Project
============================================
MODE: NORMAL OPERATION
  AIR_VALUE   = 3225
  WATER_VALUE = 1100
--------------------------------------------
Raw ADC: 3198  |  Moisture:  1%  |  Status: DRY
Raw ADC: 2450  |  Moisture: 36%  |  Status: LOW
Raw ADC: 1640  |  Moisture: 76%  |  Status: MOIST
```

---

## Production Notes

- In production firmware, sensor VCC will be switched via MOSFET to eliminate standby current draw. The sketch logic requires no changes — only the wiring source for VCC changes.
- Calibration values may drift slightly with temperature or sensor aging. Re-calibrate if readings appear inaccurate after extended deployment.
- These calibration constants feed directly into the production moisture threshold logic for Discord webhook notifications.

---

## IDE & Toolchain

| Tool | Detail |
|---|---|
| IDE | Arduino IDE |
| Board target | ESP32-WROOM-32 |
| Serial baud | 115200 |
| Version control | GitHub Desktop |

---
