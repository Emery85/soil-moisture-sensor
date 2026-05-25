# mosfet_blink_test

**Status:** PASS  
**Tests covered:** T-004, T-005  
**Date:** 2026-05-24  

---

## Purpose

Verifies that the N-channel MOSFET switches fully ON and fully OFF under ESP32 3.3 V GPIO logic. Uses an LED as the visual load — LED lighting confirms gate ON, LED dark confirms gate OFF. No bench power supply required.

---

## Hardware setup

| Component | Value | Notes |
|---|---|---|
| MOSFET | N-channel (IRLZ44N or equivalent) | Must be logic-level rated for 3.3 V Vgs |
| Load | LED + 330 Ω resistor in series | Anode → 3.3 V rail, cathode → MOSFET drain |
| Gate resistor | 10 kΩ | GPIO 2 → gate |
| Gate pull-down | 100 kΩ | Gate → GND; prevents float during boot |
| Power source | ESP32 3.3 V pin | No external supply needed |
| Control pin | GPIO 2 | |

**Circuit:** `3.3 V → 330 Ω → LED → MOSFET drain | source → GND`  
Gate driven by GPIO 2 through 10 kΩ resistor. 100 kΩ pull-down from gate to GND.

---

## Sketch

`mosfet_blink_test.ino`

Blink interval: 1000 ms. GPIO 2 toggles HIGH/LOW each cycle. Serial output confirms gate state each transition.

---

## Test results

| Test ID | Name | Result |
|---|---|---|
| T-004 | MOSFET gate ON — LED lights | PASS |
| T-005 | MOSFET gate OFF — LED dark | PASS |

**T-004 observations:** LED lit clearly on every `GATE HIGH` print. No flicker. Steady across 5+ consecutive ON cycles.  
**T-005 observations:** LED fully dark on every `GATE LOW` print. No residual glow observed.

---

## Notes

- IRLZ44N confirmed to saturate fully at 3.3 V logic — standard (non-logic-level) MOSFETs may not.
- The 100 kΩ gate pull-down is required in the production circuit to prevent gate float during ESP32 boot before GPIO initialises.
- Serial monitor must be closed before reflashing (locks COM4).
