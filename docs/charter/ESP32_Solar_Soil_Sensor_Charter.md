# Project Charter
## ESP32 Solar-Powered Soil Moisture Sensor Node

**Document Status:** Draft v0.1  
**Date:** 2026-05-11  
**Author:** Project Owner  
**Project Type:** Personal Hobby / Learning Project  
**Classification:** Non-commercial, open-source

---

## 1. Project Overview

A standalone, solar-powered soil moisture sensor node built around an ESP32 microcontroller. The device will sample capacitive soil moisture data and transmit readings wirelessly to a local home server. It is intended for permanent outdoor deployment in a raised garden bed with no grid power connection.

---

## 2. Objectives

| # | Objective | Success Criterion |
|---|-----------|-------------------|
| O-1 | Monitor soil moisture and trigger alerts | Alert fires reliably when moisture drops below configurable threshold |
| O-2 | Perpetual solar operation | Device runs ≥ 30 days without manual intervention, including cloudy-week scenarios |
| O-3 | Reasonable unit cost | Total BOM ≤ $100 USD at time of purchase |
| O-4 | Learning outcomes | Project covers embedded systems, low-power design, and IoT concepts hands-on |
| O-5 | Local-only data path | No cloud subscription services; data stays on home network |

---

## 3. Scope

### 3.1 In Scope

- ESP32-based sensor node firmware and hardware
- Capacitive soil moisture sensing
- Solar charging circuit with battery backup
- Wireless data transmission (Wi-Fi or MQTT over local network)
- Weatherproof enclosure suitable for outdoor garden use
- Power budget analysis and component sizing
- Basic data logging or Home Assistant integration

### 3.2 Out of Scope

- Cloud connectivity or paid IoT platforms
- Commercial production or sale
- Multi-node mesh networking (may be revisited post-MVP)
- Mobile app development
- PCB fabrication (breakout-board assembly assumed for v1)

---

## 4. Fixed Constraints

| Constraint | Detail |
|------------|--------|
| Microcontroller | ESP32 (already owned; no substitution) |
| Toolchain | Free and open-source software only throughout |
| Power source | Solar panel + rechargeable battery; zero grid power |
| Network | Local WiFi to Discord Notification |
| Environment | Outdoor, exposed to moisture, UV, temperature cycling |
| Budget | ≤ $100 USD total BOM |
| Use | Personal hobby only; non-commercial |

---

## 5. Open Items — ⚠️ Resolution Required

The items below are unresolved. The BOM and Technical Specification **cannot be finalized** until each is decided. Each is flagged with the downstream artifacts it blocks.

---

### ⚠️ OI-1 — Sensor Module Selection
**Status:** Open  
**Blocks:** BOM, Technical Spec (wiring, firmware ADC config)  
**Decision needed:** Choose a specific capacitive soil moisture module.

**Options to evaluate:**
- **Capacitive v1.2 clones** (AliExpress, ~$1–2): Widely used, known corrosion issues on traces. Adequate for v1.
- **STEMMA Soil Sensor (Adafruit #4026)**: I²C interface, integrated temp sensor, higher quality. ~$7.50. May exceed budget headroom.
- **DIY capacitive circuit**: Lowest cost, highest complexity; not recommended for first build.

**Recommendation:** Start with a capacitive v1.2 clone and conformal-coat the PCB traces. Revisit Adafruit STEMMA for v2 if I²C simplification is valued.

---

### ⚠️ OI-2 — Power Budget (Panel and Battery Sizing)
**Status:** Open  
**Blocks:** BOM, Technical Spec (charging circuit design)  
**Decision needed:** Determine solar panel wattage and battery capacity (mAh) based on a calculated power budget.

**Inputs required for calculation:**
- ESP32 sleep current (deep sleep ~10–150 µA depending on config)
- ESP32 active/transmit current (~160–260 mA peak)
- Sensor module current draw
- Desired sample interval (e.g., every 15 min)
- Local solar insolation estimate (Fredericksburg, VA — ~4–4.5 peak sun hours/day)
- Worst-case consecutive cloudy days to survive (suggest designing for 5 days)

**Typical result for this class of project:** 6V/1W panel + 3.7V 2000–3000 mAh LiPo. Final sizing must be confirmed by calculation before BOM is written.

---

### ⚠️ OI-3 — Data Destination and Protocol
**Status:** Open (leading candidate identified)  
**Blocks:** Technical Spec (firmware networking layer), server-side setup guide  
**Decision needed:** Confirm Home Assistant via MQTT as the data sink, or select an alternative.

**Options:**
- **Home Assistant + Mosquitto MQTT broker** ✓ *(leading candidate)*: Mature ecosystem, good ESP32 library support (PubSubClient or esp-mqtt), local-only.
- **InfluxDB + Grafana**: Better for raw time-series visualization; requires more self-hosted infrastructure.
- **HTTP POST to a local Flask/Node server**: Simpler to implement; less extensible.
- **ESPHome**: Abstracts firmware entirely; reduces learning value but accelerates deployment.

**Note:** If the learning objective is weighted heavily, a hand-written MQTT firmware (not ESPHome) is preferred. ESPHome can be used for a parallel "production" node later.

---

### ⚠️ OI-4 — Enclosure Approach
**Status:** Open  
**Blocks:** BOM (enclosure line item), physical design notes in Technical Spec  
**Decision needed:** Choose between 3D-printed enclosure and an off-the-shelf weatherproof box.

**Options:**
- **3D-printed (PETG or ASA)**: Custom fit, low material cost (~$2–4 filament), requires printer access and UV-stable filament. PLA is not acceptable outdoors.
- **Commercial IP65 junction box** (~$5–8 on Amazon/AliExpress): No printer required, proven weatherproofing, less custom fit.
- **Hybrid**: Commercial box with 3D-printed internal mounting bracket.

**Recommendation:** If a printer with PETG/ASA is available, 3D-print for best fit. Otherwise, IP65 junction box is the safe default and keeps the project moving.

---

### ⚠️ OI-5 — Firmware Framework
**Status:** Open  
**Blocks:** Technical Spec (development environment setup, library choices, build system)  
**Decision needed:** Arduino framework (via Arduino IDE or PlatformIO) vs. ESP-IDF (native Espressif SDK).

**Options:**
- **Arduino framework + PlatformIO** *(recommended for this project)*: Large library ecosystem, faster iteration, excellent deep-sleep and MQTT library support. PlatformIO adds professional project structure and dependency management vs. Arduino IDE alone.
- **ESP-IDF**: Lower-level control, steeper learning curve, better for production or advanced power optimization. Appropriate if deep low-power tuning becomes a priority in v2.

**Recommendation:** Arduino framework + PlatformIO for v1. The learning objectives (embedded + IoT) are well served; ESP-IDF can be introduced incrementally if desired.

---

## 6. Risks and Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Sensor corrosion in wet soil | Medium | Medium | Conformal coat PCB; budget for 1–2 replacements |
| Insufficient solar charging in winter | Medium | High | Size battery for 5 cloudy-day reserve; validate with power budget |
| Wi-Fi range issues (outdoor distance to router) | Low–Medium | High | Test signal strength at install location before finalizing; consider ESP32 with external antenna |
| Budget overrun | Low | Medium | Prioritize AliExpress/LCSC sourcing; defer STEMMA sensor to v2 |
| Enclosure water ingress | Low | High | Use IP65-rated seals; orient cable glands downward; test with spray |
| ESP32 brownout during transmit | Low | Medium | Add 100–470 µF capacitor on 3.3V rail; confirm LDO current rating |

---

## 7. Planned Deliverables

The following artifacts will be produced in sequence. Items marked **BLOCKED** cannot begin until the referenced Open Items are resolved.

| # | Deliverable | Depends On | Status |
|---|-------------|-----------|--------|
| D-1 | Project Charter (this document) | — | ✅ Draft |
| D-2 | Power Budget Worksheet | OI-2 inputs | ⛔ Blocked on OI-2 |
| D-3 | Bill of Materials (BOM) | OI-1, OI-2, OI-4 | ⛔ Blocked on OI-1, OI-2, OI-4 |
| D-4 | Technical Specification | OI-1 through OI-5 | ⛔ Blocked on all OIs |
| D-5 | Firmware (v1 — read, sleep, transmit) | D-4, OI-3, OI-5 | ⛔ Blocked |
| D-6 | Wiring / Assembly Guide | D-3, D-4 | ⛔ Blocked |
| D-7 | Home Assistant / MQTT Integration Notes | OI-3 | ⛔ Blocked on OI-3 |
| D-8 | Enclosure Design or Selection Guide | OI-4 | ⛔ Blocked on OI-4 |

---

## 8. Immediate Next Steps

1. **Resolve OI-5 first** (Firmware Framework) — low effort decision, unblocks Technical Spec structure.
2. **Resolve OI-3** (Data Destination) — confirm Discord Notification or select alternative.
3. **Resolve OI-1** (Sensor Module) — order or confirm a specific module so BOM line items can be priced.
4. **Complete Power Budget (D-2)** — provide your target sample interval and acceptable cloudy-day reserve; power budget calculation can then be run to determine panel/battery sizing, resolving OI-2.
5. **Resolve OI-4** (Enclosure) — confirm printer availability and filament type, or default to IP65 box.

Once OI-1 through OI-5 are closed, BOM and Technical Specification can be produced in a single pass.

---

## 9. Glossary

| Term | Definition |
|------|-----------|
| BOM | Bill of Materials — itemized parts list with quantities and costs |
| ESP32 | Espressif ESP32 series dual-core Wi-Fi/BT microcontroller |
| MQTT | Message Queuing Telemetry Transport — lightweight IoT messaging protocol |
| Deep Sleep | ESP32 low-power mode; most peripherals off; wakes on timer or interrupt |
| IP65 | Ingress Protection rating: dust-tight, water jet resistant |
| LiPo | Lithium Polymer rechargeable battery |
| PlatformIO | Open-source embedded development platform / build system |
| PETG / ASA | UV- and moisture-resistant 3D printing filaments suitable for outdoor use |
| ADC | Analog-to-Digital Converter — used to read analog soil sensor voltage |

---

*End of Document — ESP32 Solar Soil Sensor Project Charter v0.1*
