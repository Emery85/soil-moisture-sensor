# ESP32 Solar-Powered Soil Moisture Sensor

A standalone, solar-powered soil moisture sensor node built around an ESP32 microcontroller.
Samples capacitive soil moisture data and sends alerts via Discord webhook to a home network.
Intended for permanent outdoor deployment in a raised garden bed — no grid power, no cloud services.

## Hardware

| Component | Selection |
|-----------|-----------|
| Microcontroller | ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1) |
| Sensor | Capacitive moisture module v1.2 (MOSFET-switched) |
| Solar panel | 6V 3W 500mA |
| Battery | 3.7V 18650 3400mAh |
| Charging IC | TP4056 with DW01A protection module (Type-C input) |
| Enclosure | IP65-rated junction box |

## Key Design Decisions

- **12-hour sample interval** — soil changes slowly; minimizes energy use
- **MOSFET sensor power switching** — eliminates parasitic drain during deep sleep
- **Discord webhook** — no broker or server infrastructure required; native mobile push notifications
- **Arduino framework** — large library ecosystem, fast iteration for v1

## Project Status

Firmware complete. Bench-tested; pending outdoor deployment.

## Repository Structure

```
firmware/
  soil_moisture_monitor_production/  — production firmware
  tests/                             — test sketches (T-001 through T-008)
libraries/
  SoilSensor/                        — custom SoilSensor library
config/
  config_template.h                  — copy to config.h and fill in credentials
docs/
  charter/                           — project charter
  decisions/                         — decision log
  specs/                             — technical specification
  bom/                               — bill of materials
  schematics/                        — circuit schematics (SVG)
  datasheets/                        — component datasheets
  research/                          — component notes
```

## Getting Started

1. Copy `config/config_template.h` to `firmware/soil_moisture_monitor_production/config.h`
2. Fill in your Wi-Fi credentials and Discord webhook URL
3. Open `firmware/soil_moisture_monitor_production/soil_moisture_monitor_production.ino` in Arduino IDE
4. Flash to ESP32-WROOM-32

> `config.h` is gitignored — never commit credentials.

## License

MIT — see [LICENSE](LICENSE).
