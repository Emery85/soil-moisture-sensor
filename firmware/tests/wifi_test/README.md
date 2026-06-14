# WiFi Test

**Status:** Superseded — sketch not implemented  
**Location:** `firmware/tests/wifi_test/`

---

## Status Note

A standalone WiFi connectivity test sketch was planned here but was not written as a separate test. WiFi functionality was validated as part of the Discord webhook test (`firmware/tests/discord_test/`), which exercises the full connection path: WiFi connect → NTP sync → HTTP POST → disconnect.

See `firmware/tests/discord_test/README.md` for the test that covers WiFi.

---

## What the Discord Test Validates for WiFi

| Check | Detail |
|---|---|
| SSID association | Connects to 2.4 GHz network within 10 seconds |
| IP address assignment | DHCP address printed to Serial |
| Signal strength | RSSI reported in dBm |
| Internet reachability | HTTP POST reaches Discord API |
| Clean disconnect | `WiFi.disconnect(true)` + `WiFi.mode(WIFI_OFF)` |

---

## ADC2 / WiFi Conflict — Important Note

The ESP32 ADC2 (GPIO0, 2, 4, 12–15, 25–27) **cannot be used while WiFi is active**. All sensor readings in this project use GPIO34, which is on ADC1 and has no conflict with the WiFi radio. This constraint is enforced in both the test firmware and production firmware.
