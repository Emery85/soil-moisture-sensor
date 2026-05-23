# ESP32 Solar-Powered Soil Moisture Sensor

A standalone, solar-powered soil moisture sensor node built around an ESP32 microcontroller.
Samples capacitive soil moisture data and sends alerts via Discord webhook to a home network.
Intended for permanent outdoor deployment in a raised garden bed — no grid power, no cloud services.

## Hardware

| Component | Selection |
|-----------|-----------|
| Microcontroller | ESP32 |
| Sensor | Capacitive moisture module v1.2 (MOSFET-switched) |
| Solar panel | 6V 3W 500mA |
| Battery | 3.7V 18650 3400mAh |
| Charging IC | CN3065 solar LiPo charger module |
| Enclosure | IP65-rated junction box |

## Key Design Decisions

- **12-hour sample interval** — soil changes slowly; minimizes energy use
- **MOSFET sensor power switching** — eliminates parasitic drain during deep sleep
- **Discord webhook** — no broker or server infrastructure required; native mobile push notifications
- **Arduino framework** — large library ecosystem, fast iteration for v1

## Project Status

Firmware in development. See [`docs/`](docs/) for full project documentation.

## Repository Structure

```
firmware/         — Arduino sketches (production + test suites)
libraries/        — Custom SoilSensor library
config/           — config_template.h (copy to config.h and fill in credentials)
docs/
  charter/        — Project charter
  decisions/      — Decision log
  specs/          — Technical specification
  bom/            — Bill of materials
  research/       — Component notes
  schematics/     — (placeholder)
```

## Getting Started

1. Copy `config/config_template.h` to `config/config.h`
2. Fill in your Wi-Fi credentials and Discord webhook URL
3. Open `firmware/production/production.ino` in Arduino IDE or PlatformIO
4. Flash to ESP32

> `config/config.h` is gitignored — never commit credentials.
