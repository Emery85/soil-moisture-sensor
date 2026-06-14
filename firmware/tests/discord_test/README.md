# Discord Webhook Notification Test

**Status:** Complete  
**Location:** `firmware/tests/discord_test/`  
**Sketch:** `discord_test.ino`

---

## Purpose

Validates end-to-end Discord webhook delivery over WiFi. This test exercises the full notification path — WiFi connection, JSON payload construction, HTTP POST to the Discord webhook API, and response code validation — without involving any sensors or hardware beyond the ESP32 itself.

Passing this test confirms the notification subsystem is ready for integration into production firmware.

---

## Pass Criteria

| # | Criterion | Expected Result |
|---|---|---|
| 1 | WiFi connection | Connected within 10 seconds |
| 2 | HTTP POST response | `204 No Content` |
| 3 | Discord channel | Test message appears in the channel |
| 4 | Post-test state | Board enters deep sleep; current drops |

---

## Prerequisites

### Discord Webhook URL

Before flashing, you must have a Discord webhook URL for your target channel.

**To create one:**
1. Open Discord → go to the server and channel you want alerts in
2. Click the gear icon next to the channel name → **Edit Channel**
3. Go to **Integrations** → **Webhooks** → **New Webhook**
4. Name it (e.g. `Garden Sensor`) and click **Copy Webhook URL**

**To verify the webhook before flashing** (optional):

```
curl -X POST -H "Content-Type: application/json" \
  -d "{\"content\": \"Test from curl\"}" \
  https://discord.com/api/webhooks/YOUR_ID/YOUR_TOKEN
```

Expected: no output, no error (Discord returns `204 No Content` with an empty body).

### Arduino Libraries

Install via **Tools → Manage Libraries** in Arduino IDE:

| Library | Author | Version |
|---|---|---|
| ArduinoJson | Benoit Blanchon | >= 6.x |

`WiFi` and `HTTPClient` are bundled with the ESP32 Arduino core — no separate install needed.

---

## Configuration

Open `discord_test.ino` and fill in the three values in the `CONFIG` section near the top:

```cpp
const char* WIFI_SSID       = "YOUR_SSID";
const char* WIFI_PASSWORD   = "YOUR_PASSWORD";
const char* DISCORD_WEBHOOK = "https://discord.com/api/webhooks/YOUR_ID/YOUR_TOKEN";
```

> **Do not commit credentials to Git.** Fill in locally, flash, and do not stage the `.ino` file with real values in it.

---

## How to Flash

1. Open `discord_test.ino` in Arduino IDE
2. Fill in credentials in the CONFIG section
3. **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
4. **Tools → Port → COM4** (CP210x — not COM3)
5. Click **Upload** and wait for `Done uploading`
6. **Tools → Serial Monitor** → set baud to **115200**
7. Press **EN/Reset** on the board if output doesn't appear
8. Check your Discord channel for the test message
9. Close Serial Monitor before any subsequent flash

---

## Expected Serial Output

```
============================================
  Discord Webhook Notification Test
  Solar Soil Moisture Monitor - Rev A
============================================

[STEP 1] Connecting to Wi-Fi: YourNetworkName
........
[PASS] Wi-Fi connected.
       IP Address : 192.168.1.xxx
       Signal (RSSI): -62 dBm

[STEP 2] Building Discord payload...
         Payload: {"content":"✅ **Soil Moisture Monitor ..."}

[STEP 3] Sending HTTP POST to Discord webhook...
         HTTP Response Code: 204
[PASS] HTTP 204 received — message delivered to Discord.
       Check your Discord channel to confirm the message.

[STEP 4] Disconnecting Wi-Fi...
[DONE]   Wi-Fi disconnected.

============================================
  Test Complete
  Result: PASS
============================================

[SLEEP]  Entering deep sleep for 30 seconds...
         (Disconnect Serial Monitor before next flash)
```

---

## Failure Modes & Troubleshooting

| Symptom | Likely Cause | Action |
|---|---|---|
| `Wi-Fi connection timed out` | Wrong SSID/password, or 5 GHz network | Verify credentials; ESP32 supports 2.4 GHz only |
| HTTP response `401` | Invalid or expired webhook URL | Regenerate webhook in Discord settings |
| HTTP response `404` | Webhook URL malformed or channel deleted | Check URL — ensure it is pasted in full |
| HTTP POST failed (negative code) | No internet, DNS failure | Confirm router has internet access |
| No output in Serial Monitor | Wrong baud rate or board not reset | Close and reopen at 115200; press EN/Reset |
| Board won't flash | Serial Monitor still open (locks COM port) | Close Serial Monitor, then flash |

---

## Deep Sleep Note

The board enters deep sleep for 30 seconds after the test completes. This mirrors production firmware behavior and validates that deep sleep entry works from this code path.

To reflash during the sleep window, hold the **BOOT** button on the ESP32 while clicking **Upload**.
