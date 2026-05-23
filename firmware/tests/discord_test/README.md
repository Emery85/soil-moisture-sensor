# Discord Webhook Notification Test

**Project:** Solar-Powered Soil Moisture Monitor  
**Firmware Phase:** Test — T-03 Discord Webhook  
**File:** `firmware/tests/discord\\\_test/Discord\\\_test.ino`  
**Revision:** A

\---

## Purpose

Validates end-to-end Discord webhook delivery over Wi-Fi. This test exercises the full notification path — Wi-Fi connection, JSON payload construction, HTTP POST to the Discord webhook API, and response code validation — without involving any sensors or hardware beyond the ESP32 itself.

Passing this test confirms the notification subsystem is ready for integration into the production firmware.

\---

## Pass Criteria

|#|Criterion|Expected Result|
|-|-|-|
|1|Wi-Fi connection|Connected within 10 seconds|
|2|HTTP POST response|`204 No Content`|
|3|Discord channel|Test message appears in the channel|
|4|Post-test state|Board enters deep sleep; current drops|

\---

## Prerequisites

### Discord Webhook URL

Before flashing, you must have a Discord webhook URL for your target channel.

**To create one:**

1. Open Discord → go to the server and channel you want alerts in
2. Click the gear icon next to the channel name → **Edit Channel**
3. Go to **Integrations** → **Webhooks** → **New Webhook**
4. Name it (e.g. `Garden Sensor`) and click **Copy Webhook URL**

**To verify the webhook before flashing** (optional but recommended):

Open a command prompt and run:

```
curl -X POST -H "Content-Type: application/json" -d "{\\\\"content\\\\": \\\\"Test from curl\\\\"}" https://discord.com/api/webhooks/YOUR\\\_ID/YOUR\\\_TOKEN
```

Expected: no output, no error (Discord returns `204 No Content` with an empty body).

### Arduino Libraries

Install the following via **Tools → Manage Libraries** in Arduino IDE:

|Library|Author|Version|
|-|-|-|
|ArduinoJson|Benoit Blanchon|>= 6.x|

`WiFi` and `HTTPClient` are bundled with the ESP32 Arduino core — no separate install needed.

\---

## Configuration

Open `Discord\\\_test.ino` and fill in the three values in the `CONFIG` section near the top of the file:

```cpp
const char\\\* WIFI\\\_SSID       = "YOUR\\\_SSID";
const char\\\* WIFI\\\_PASSWORD   = "YOUR\\\_PASSWORD";
const char\\\* DISCORD\\\_WEBHOOK = "https://discord.com/api/webhooks/YOUR\\\_ID/YOUR\\\_TOKEN";
```

> ⚠️ \\\*\\\*Do not commit credentials to Git.\\\*\\\* The `config/` directory in this repo is gitignored for exactly this reason. Fill in credentials locally, flash the board, and do not stage or push the `.ino` file with real values in it.

\---

## Flash \& Run Instructions

1. Pull latest from GitHub Desktop
2. Open `firmware/tests/discord\\\_test/Discord\\\_test.ino` in Arduino IDE
3. Fill in credentials in the CONFIG section (see above)
4. **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
5. **Tools → Port → COM4** (CP210x — not COM3)
6. Click **Upload** and wait for `Done uploading`
7. **Tools → Serial Monitor** → set baud to **115200**
8. Press **EN/Reset** on the board if output doesn't appear immediately
9. Observe the Serial Monitor output (see expected output below)
10. Check your Discord channel for the test message
11. Close Serial Monitor before attempting any subsequent flash

\---

## Expected Serial Output

```
============================================
  Discord Webhook Notification Test
  Solar Soil Moisture Monitor - Rev A
============================================

\\\[STEP 1] Connecting to Wi-Fi: YourNetworkName
........
\\\[PASS] Wi-Fi connected.
       IP Address : 192.168.1.xxx
       Signal (RSSI): -62 dBm

\\\[STEP 2] Building Discord payload...
         Payload: {"content":"✅ \\\*\\\*Soil Moisture Monitor ..."}

\\\[STEP 3] Sending HTTP POST to Discord webhook...
         HTTP Response Code: 204
\\\[PASS] HTTP 204 received — message delivered to Discord.
       Check your Discord channel to confirm the message.

\\\[STEP 4] Disconnecting Wi-Fi...
\\\[DONE]   Wi-Fi disconnected.

============================================
  Test Complete
  Result: PASS
============================================

\\\[SLEEP]  Entering deep sleep for 30 seconds...
         (Disconnect Serial Monitor before next flash)
```

\---

## Failure Modes \& Troubleshooting

|Symptom|Likely Cause|Action|
|-|-|-|
|`Wi-Fi connection timed out`|Wrong SSID/password, or 5 GHz network|Verify credentials; ESP32 supports 2.4 GHz only|
|HTTP response `401`|Invalid or expired webhook URL|Regenerate webhook in Discord settings|
|HTTP response `404`|Webhook URL malformed or channel deleted|Check URL — ensure it is pasted in full|
|HTTP POST failed (negative code)|No internet, DNS failure|Confirm router has internet; try a different network|
|No output in Serial Monitor|Serial Monitor opened before board reset, or wrong baud|Close and reopen at 115200; press EN/Reset|
|Board won't flash|Serial Monitor still open (locks COM port)|Close Serial Monitor, then flash|

\---

## Deep Sleep Note

The board enters deep sleep for 30 seconds after the test completes. This is intentional — it mirrors the production firmware behavior and validates that deep sleep entry works correctly from this code path.

To reflash during the sleep window, hold the **BOOT** button on the ESP32 while clicking **Upload** in Arduino IDE.

\---

## Next Steps

Once this test passes:

* \[ ] Commit `Discord\\\_test.ino` to `firmware/tests/discord\\\_test/` (without credentials)
* \[ ] Log result in the project decision/test log
* \[ ] Proceed to `mosfet\\\_test` — MOSFET control and sensor power switching

\---

## Revision History

|Rev|Date|Author|Change|
|-|-|-|-|
|A|2026-05|Andrew|Initial release|



