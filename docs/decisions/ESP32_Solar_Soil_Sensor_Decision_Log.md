# Decision Log
## ESP32 Solar-Powered Soil Moisture Sensor Node

**Document Status:** Active — update as new decisions are made  
**Date Created:** 2026-05-13  
**Related Documents:** Project Charter v0.1, Power Budget Calculation

---

## How to Use This Log

Add a row whenever a meaningful design, component, or architecture decision is made. Capture the alternatives that were considered — this is the primary value of the log when revisiting choices later. Decisions should not be deleted; if a decision is reversed, add a new row and cross-reference the original.

---

## Decision Log

| ID | Date | Topic | Decision | Alternatives Considered | Rationale | Status |
|----|------|-------|----------|------------------------|-----------|--------|
| DL-01 | 2026-05-13 | Microcontroller | ESP32 | ESP8266, Arduino + separate Wi-Fi module, RP2040 + Wi-Fi | Already owned; integrated Wi-Fi; strong community and library support; deep sleep capability adequate for low-power design goals | ✅ Closed |
| DL-02 | 2026-05-13 | Soil Moisture Sensor | Arduino-style capacitive moisture sensor module (10-pack, ~$1.20/unit) | Adafruit STEMMA Soil Sensor (I²C, ~$7.50); DIY capacitive circuit; resistive sensor modules | Lowest cost per unit; adequate accuracy for threshold-based alerting; known failure mode (trace corrosion) mitigated by MOSFET-switching sensor power off during sleep; v2 can upgrade to STEMMA if I²C simplification is valued | ✅ Closed |
| DL-03 | 2026-05-13 | Parasitic Drain Mitigation | MOSFET switch on sensor power rail | Always-on sensor power; software-only power management | Capacitive sensor modules draw current continuously when powered; MOSFET cuts power during deep sleep, eliminating parasitic drain entirely; negligible BOM cost addition | ✅ Closed |
| DL-04 | 2026-05-13 | Solar Panel | 6V 3W 500mA panel | 6V 1W; 5V USB solar panel; 12V panel with buck converter | Power budget calculation confirms 1,827 mAh/day harvest at 4.3 peak sun hours (Fredericksburg, VA) vs ~1.33 mAh/day consumption — substantial margin; 6V nominal appropriate for TP4056 input range; already selected by owner | ✅ Closed |
| DL-05 | 2026-05-13 | Battery | 3.7V 18650, 3400mAh | 2000mAh 18650; LiPo pouch cells (various); AA alkaline with boost converter | Power budget confirms ~2,045 days theoretical autonomy at rated capacity (80% usable = 2,720mAh ÷ 1.33mAh/day); 18650 format robust, widely available, rechargeable; 3400mAh provides extreme cloudy-day reserve well beyond any realistic outage | ✅ Closed |
| DL-06 | 2026-05-13 | Sample Interval | 12 hours (2 readings/day) | 1 hour; 6 hours; 24 hours | Soil moisture in a raised bed changes slowly; 12-hour interval captures morning and evening states; dramatically reduces daily energy consumption vs shorter intervals; adequate for threshold alerting use case | ✅ Closed |
| DL-07 | 2026-05-13 | Data Destination & Protocol | Discord webhook via HTTP POST | Home Assistant + Mosquitto MQTT; InfluxDB + Grafana; ESPHome; HTTP POST to local Flask/Node server | No broker or server infrastructure required; free tier; instant mobile push notifications natively; ESP32 HTTP POST straightforward to implement with ArduinoHttpClient or HTTPClient library; sufficient for single-node alerting use case | ✅ Closed |
| DL-08 | 2026-05-13 | Enclosure | COTS IP65-rated junction box | 3D-printed PETG enclosure; 3D-printed ASA enclosure; hybrid (commercial box + printed internal mount) | Proven weatherproofing without printer dependency; faster to deploy; IP65 rating covers outdoor garden environment; eliminates UV-degradation risk of printed parts; lower effort for v1 | ✅ Closed |
| DL-09 | 2026-05-13 | Firmware Framework | Arduino framework | ESP-IDF (native Espressif SDK); ESPHome; MicroPython | Large library ecosystem (HTTPClient, ArduinoJson, deep sleep APIs); fastest iteration for v1; well-documented for ESP32 deep sleep and Wi-Fi use cases; ESPHome excluded to preserve firmware learning objective; ESP-IDF deferred to v2 if low-level power tuning becomes necessary | ✅ Closed |
| DL-10 | 2026-05-13 | Power Budget Outcome | Panel and battery pairing confirmed — no redesign required | Larger battery; smaller panel; shorter sample interval | Calculated daily consumption ~1.33mAh/day; daily solar harvest ~1,827mAh/day; usable battery capacity 2,720mAh; theoretical no-sun autonomy ~2,045 days; all margins healthy; 18650 self-discharge (~1–3%/month) is a larger factor than operational load; charging IC selection (TP4056 with protection) is the remaining power subsystem decision | ✅ Closed |

| DL-11 | 2026-05-13 | Charging IC / Solar Charger Module | ~~HiLetgo CN3065 Mini Solar LiPo Charger Board (2-pack)~~ **SUPERSEDED by DL-13** | TP4056 with DW01+FS8205 protection module; bare TP4056; MCP73831 | Solar-specific IC with undervoltage lockout and input conditioning suited to panel voltage variation; pre-built module with supporting passives, status LEDs, and SYS OUT power rail; supports up to 1000mA charge current — panel's 500mA is well within range; Micro USB backup charging input included; note: input voltage minimum is 6.5V — marginally above 6V panel nominal, confirmed acceptable under real-world panel loaded voltage (typically 6–7V under direct sun) | ⚠️ Superseded |
| DL-12 | 2026-05-13 | 18650 Over-Discharge Protection | Excluded — unprotected 18650 cell accepted without separate protection circuit | Protected 18650 cell (built-in PCB); external DW01+FS8205 protection module; software low-voltage cutoff via ESP32 ADC monitoring | Power budget confirms ~2,045 days theoretical autonomy on a full charge at operational load of ~1.33mAh/day; probability of reaching over-discharge threshold (~2.5V) under normal solar operation is negligible; self-discharge (~1–3%/month) is the dominant depletion mechanism, not load; CN3065 undervoltage lockout provides partial protection by halting charging below threshold; risk accepted for v1 — revisit if sample interval is shortened significantly in future revisions | ✅ Closed |
| DL-13 | 2026-05-24 | Charging IC / Solar Charger Module (revision) | Standard TP4056 with protection module (Type-C, 6-pack, B08FSRV7GS) — supersedes DL-11 | HW-736 integrated solar charge management board (already procured, both units failed); CN3065 HiLetgo module (DL-11 selection, also procured as HW-736) | Both HW-736 units from original order overheated on first use during T-009a bench verification — board components reached 140–200°F with battery connected; one unit emitted smoke; battery remained cool confirming cell is undamaged; failure attributed to board-level manufacturing defect, not wiring or configuration error. Standard TP4056 two-chip module (TP4056 + DW01A) selected as replacement: well-documented, proven in this application class, widely available, Type-C input preferred over micro-USB. Solar panel connects to IN+/IN− solder pads; battery to B+/B−; LDO input to OUT+/OUT−. BOM to be updated: HW-736 line marked obsolete/failed, new module added as replacement. | ✅ Closed |
| DL-14 | 2026-05-24 | LDO Output Capacitor Configuration | Dual capacitor: 22µF at LDO output pin + 470µF at ESP32 3V3 pin, both on the 3.3V rail | Single 470µF only; single 22µF only; no output capacitor | Two capacitors serve distinct purposes and are not interchangeable despite being on the same node. 22µF is required by AMS1117 datasheet for output stability — without it the LDO feedback loop can oscillate regardless of downstream capacitance. 470µF is a bulk decoupling reservoir to absorb ESP32 Wi-Fi transmit spikes (~240mA peak) and prevent brownout resets. In a compact layout both caps are in parallel on the 3.3V/GND node (492µF total) but placement matters: 22µF physically close to LDO output pin; 470µF physically close to ESP32 3V3 pin. Dropping the 22µF risks LDO instability due to trace inductance between LDO and the more distant 470µF. Both values are available in standard capacitor assortment packs at negligible cost. | ✅ Closed |

---

## Pending Decisions

None — all decisions closed. BOM and Technical Specification are unblocked.

---

## Change History

| Version | Date | Change |
|---------|------|--------|
| 0.1 | 2026-05-13 | Initial log created; DL-01 through DL-10 back-populated from project initialization and OI resolution; DL-11 added as pending |
| 0.2 | 2026-05-13 | DL-11 closed: CN3065 HiLetgo module selected as solar charging IC; DL-12 added and closed: over-discharge protection excluded based on power budget risk assessment |
| 0.3 | 2026-05-24 | DL-11 superseded: HW-736 boards failed on first use during T-009a bench verification (thermal runaway, smoke); DL-13 added and closed: standard TP4056 with protection (Type-C) selected as replacement charging module; DL-14 added and closed: dual capacitor configuration confirmed (22µF at LDO output + 470µF at ESP32 3V3 pin) |

---

*End of Document — ESP32 Solar Soil Sensor Decision Log v0.3*
