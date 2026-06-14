# Project Charter
## ESP32 Solar-Powered Soil Moisture Sensor Node

**Document Status:** Complete v1.0  
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

## 5. Open Items — All Resolved

All open items were resolved during design and bench testing. See the decision log (`docs/decisions/`) for full rationale on each.

---

### ✅ OI-1 — Sensor Module Selection
**Status:** Resolved → DL-02  
**Decision:** Capacitive moisture sensor v1.2 clone. Calibrated during T-008; constants (dry: 3225, wet: 1100) carried into production firmware.

---

### ✅ OI-2 — Power Budget (Panel and Battery Sizing)
**Status:** Resolved → DL-04, DL-05, DL-10  
**Decision:** 6V 3W 500mA panel + 3.7V 18650 3400mAh. Power budget confirmed ~1.33 mAh/day operational load vs ~1,827 mAh/day solar harvest. Theoretical no-sun autonomy ~2,045 days.

---

### ✅ OI-3 — Data Destination and Protocol
**Status:** Resolved → DL-07  
**Decision:** Discord webhook via HTTP POST. No broker or server infrastructure required; native mobile push notifications; straightforward to implement with ESP32 HTTPClient library.

---

### ✅ OI-4 — Enclosure Approach
**Status:** Resolved → DL-08  
**Decision:** COTS IP65-rated junction box. Proven weatherproofing, no printer dependency, faster to deploy for v1.

---

### ✅ OI-5 — Firmware Framework
**Status:** Resolved → DL-09  
**Decision:** Arduino framework via Arduino IDE. PlatformIO excluded — incompatible with ESP32-D0WD-V3 rev 3.1 for this project. ESP-IDF deferred to v2.

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
| D-1 | Project Charter (this document) | — | ✅ Complete |
| D-2 | Power Budget Worksheet | OI-2 inputs | ✅ Complete — see DL-10 |
| D-3 | Bill of Materials (BOM) | OI-1, OI-2, OI-4 | ✅ Complete — `docs/bom/` |
| D-4 | Technical Specification | OI-1 through OI-5 | ✅ Complete — `docs/specs/` |
| D-5 | Firmware (v1 — read, sleep, transmit) | D-4, OI-3, OI-5 | ✅ Complete — `firmware/soil_moisture_monitor_production/` |
| D-6 | Wiring / Assembly Guide | D-3, D-4 | ✅ Complete — `docs/schematics/` |
| D-7 | Discord Integration Notes | OI-3 | ✅ Complete — documented in production firmware README |
| D-8 | Enclosure Selection | OI-4 | ✅ Complete — IP65 junction box selected (DL-08) |

---

## 8. Project Outcome

All open items resolved. All deliverables complete. Production firmware bench-tested and pending outdoor deployment in the raised garden bed (Fredericksburg, VA).

**Final configuration summary:**
- ESP32-WROOM-32, Arduino IDE toolchain
- Capacitive moisture sensor v1.2 on GPIO34, MOSFET-switched
- 6V 3W solar panel + 3.7V 18650 3400mAh + TP4056/DW01A charger
- Discord webhook alerts at 8:00 AM and 8:00 PM EDT via deep sleep scheduling
- IP65 junction box enclosure

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
