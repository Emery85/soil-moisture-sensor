# CLAUDE.md — ESP32 Solar Soil Moisture Sensor

Project-specific context for Claude Code sessions.

---

## Project Overview

Standalone, solar-powered soil moisture sensor node built on an ESP32-WROOM-32.
Reads capacitive soil moisture and sends threshold alerts via Discord webhook to a home network.
Personal hobby / learning project. Non-commercial, open-source.

**GitHub:** https://github.com/Emery85/soil-moisture-sensor  
**Status:** Firmware in development — test phase in progress.

---

## Repository Structure

```
firmware/
  production/         — production.ino (main firmware, in progress)
  tests/
    boot_test/        — T-001 through T-003 (boot, WiFi, Discord) — status unknown
    mosfet_test/      — T-004, T-005 — PASS (2026-05-24)
    wifi_test/        — status unknown
    discord_test/     — status unknown
    capacitor_sensor_test/ — calibration complete (2026-05-24); see below
libraries/
  SoilSensor/         — custom library (SoilSensor.h / .cpp)
config/
  config_template.h   — template; copy to config.h and fill in credentials
docs/
  charter/            — project charter (ESP32_Solar_Soil_Sensor_Charter.md)
  decisions/          — decision log v0.3 (ESP32_Solar_Soil_Sensor_Decision_Log.md)
  specs/              — technical specification (.docx)
  bom/                — bill of materials (BOM_Procurement.xlsx)
  research/           — component notes
  schematics/         — SVG schematics for each test circuit
```

---

## Toolchain

- **IDE:** Arduino IDE — **not PlatformIO**. The board (ESP32-D0WD-V3 rev 3.1) is
  incompatible with PlatformIO for this project.
- **Board target:** ESP32-WROOM-32
- **Serial baud:** 115200
- **ADC rule:** Always use ADC1 pins (e.g. GPIO34). ADC2 conflicts with the WiFi radio.

---

## Hardware Configuration (v1)

| Component | Selection |
|-----------|-----------|
| Microcontroller | ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1) |
| Sensor | Capacitive moisture module v1.2 — MOSFET-switched power |
| Solar panel | 6V 3W 500mA |
| Battery | 3.7V 18650 3400mAh (unprotected cell) |
| Charging IC | TP4056 with protection module, Type-C input |
| Enclosure | IP65-rated junction box |
| Data destination | Discord webhook via HTTP POST |
| Sample interval | 12 hours (deep sleep between readings) |

**Note:** The original CN3065 / HW-736 charging board selection (DL-11) was superseded after
both units failed on first use during bench test T-009a (thermal runaway, smoke). Standard
TP4056 with protection selected as replacement (DL-13). See decision log for full detail.

**Capacitor config (DL-14):** 22µF at LDO output pin + 470µF at ESP32 3V3 pin (both required —
they serve different purposes; do not substitute one for the other).

---

## Calibration Values (capacitor_sensor_test)

Sensor: capacitive v1.2 module. Board: ESP32-WROOM-32. Pin: GPIO34.

| Condition | Raw ADC |
|-----------|---------|
| Dry air | **3225** |
| Submerged in water | **1100** |

These constants are set in `firmware/tests/capacitor_sensor_test/capacitor_sensor_test.ino`
and will carry forward into production firmware.

---

## Security — Never Commit

`config/config.h` is gitignored. It holds Wi-Fi credentials and the Discord webhook URL.
Only `config/config_template.h` (with placeholder values) is tracked.

---

## Git & GitHub

- **Remote:** https://github.com/Emery85/soil-moisture-sensor
- **Default branch:** `main`
- **GitHub CLI:** installed (`gh`), authenticated as `Emery85`
- **Git user:** Emery85 / 106268954+Emery85@users.noreply.github.com

When committing binary files (`.xlsx`, `.docx`, `.svg`) include them normally — they are
intentionally tracked. The `.~lock.*.xlsx#` lock file pattern is gitignored.

---

## Open Issues

- [ ] [#1](https://github.com/Emery85/soil-moisture-sensor/issues/1) — Add license body to LICENSE file (MIT or GPL-3.0 recommended)
