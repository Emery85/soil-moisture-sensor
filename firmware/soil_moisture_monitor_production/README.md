# Soil Moisture Monitor — Production Firmware

**Status:** Complete — pending outdoor deployment  
**Location:** `firmware/soil_moisture_monitor_production/`  
**Sketch:** `soil_moisture_monitor_production.ino`

---

## What It Does

Wakes at 8:00 AM and 8:00 PM EDT, reads soil moisture from a capacitive sensor, sends a formatted alert to a Discord channel via webhook, then returns to deep sleep until the next scheduled window.

Each Discord alert includes:
- Moisture tier label and emoji (DRY / LOW / MODERATE / MOIST / WET)
- Moisture percentage
- A visual bar indicator
- Timestamp (EDT)
- Color-coded embed matching the tier

---

## Hardware

| Component | Detail |
|---|---|
| Microcontroller | ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1) |
| Moisture sensor | Capacitive v1.2 module on GPIO34 (ADC1_CH6) |
| Sensor power switch | N-channel MOSFET on GPIO26, 330Ω gate resistor, 10kΩ pull-down |
| Charging | TP4056 + DW01A protection module (Type-C input) |
| Battery | 3.7V 18650 3400mAh |
| Solar panel | 6V 3W 500mA |
| LDO decoupling | 22µF at LDO output pin + 470µF at ESP32 3V3 pin |

---

## Calibration

Calibrated against this specific sensor and board during test T-008:

| Condition | Raw ADC |
|---|---|
| Dry air | 3225 |
| Submerged in water | 1100 |

Readings are averaged over 10 samples and mapped linearly to 0–100%.

---

## Moisture Tiers

| Range | Label | Emoji |
|---|---|---|
| 0–19% | DRY | 🔴 |
| 20–39% | LOW | 🟠 |
| 40–59% | MODERATE | 🟡 |
| 60–79% | MOIST | 🟢 |
| 80–100% | WET | 🔵 |

---

## Schedule

The firmware targets 8:00 AM and 8:00 PM **EDT (UTC−4)**. Times are stored as UTC internally:

| EDT | UTC |
|---|---|
| 8:00 AM | 12:00 |
| 8:00 PM | 00:00 |

On each wakeup, the firmware:
1. Powers the sensor via MOSFET
2. Connects to WiFi (3 attempts, 15s timeout each)
3. Syncs time via NTP (`pool.ntp.org`)
4. Reads and averages 10 ADC samples
5. POSTs a Discord embed via webhook (HTTP 204 = success)
6. Powers off sensor, disconnects WiFi
7. Calculates seconds until the next target window and enters deep sleep

**WiFi failure fallback:** sleeps 30 minutes and retries.  
**NTP failure fallback:** sleeps 6 hours and retries.

> **Note:** The UTC offset is hardcoded to EDT (UTC−4). Readings will shift one hour when daylight saving ends in November. Reflash with `TARGET_HOUR_1_UTC 13` / `TARGET_HOUR_2_UTC 1` for EST if needed.

---

## Configuration

1. Copy `config_template.h` to `config.h` in this folder
2. Fill in your WiFi credentials and Discord webhook URL:

```cpp
#define WIFI_SSID           "your-network-name"
#define WIFI_PASSWORD       "your-password"
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/YOUR_ID/YOUR_TOKEN"
```

> `config.h` is gitignored — never commit credentials.

**To get a Discord webhook URL:** Server Settings → Integrations → Webhooks → New Webhook → Copy Webhook URL.

---

## How to Flash

1. Open `soil_moisture_monitor_production.ino` in Arduino IDE
2. **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. **Tools → Port → COM4** (CP210x)
4. Click **Upload**
5. Open **Tools → Serial Monitor** at 115200 baud to observe the first wakeup cycle

---

## Expected Serial Output

```
========================================
 Soil Moisture Monitor — Wakeup
========================================
[MOSFET] Sensor power ON.
[WiFi] Attempt 1/3
[WiFi] Connecting to YourNetwork........
[WiFi] Connected. IP: 192.168.1.xxx
[NTP] UTC: 12:00  →  EDT: 08:00
[Sensor] Raw ADC avg: 2140  →  Moisture: 51%
[Reading] 51% — 🟡 MODERATE
[Discord] HTTP 204
[Discord] Alert sent successfully.
[MOSFET] Sensor power OFF.
[WiFi] Disconnected.
[Sleep] Sleeping for 43200 seconds (~12.0 hours).
[Sleep] Entering deep sleep. Goodnight.
========================================
```

---

## Dependencies

| Library | Install via |
|---|---|
| ArduinoJson (>= 6.x) | Arduino Library Manager |
| WiFi | Bundled with ESP32 core |
| HTTPClient | Bundled with ESP32 core |

---

## Related

- `firmware/tests/` — individual subsystem tests run before this firmware was written
- `docs/decisions/` — design rationale for scheduling, power, component selection
- `docs/schematics/` — wiring diagrams
