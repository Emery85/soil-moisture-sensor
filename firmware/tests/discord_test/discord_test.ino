// ============================================================
// Discord Webhook Notification Test
// Project: Solar-Powered Soil Moisture Monitor
// File:    firmware/tests/discord_test/Discord_test.ino
// Author:  Andrew
// Rev:     A
//
// Purpose:
//   Validates end-to-end Discord webhook delivery over Wi-Fi.
//   Connects to the network, POSTs a test message to the
//   configured webhook URL, confirms HTTP 204 response, then
//   enters deep sleep. No sensors or MOSFETs are exercised.
//
// Pass Criteria:
//   - Wi-Fi connects within 10 s
//   - HTTP POST returns 204 No Content
//   - Test message appears in the Discord channel
//   - Deep sleep entered; current drops to near-zero
//
// Hardware Required:
//   - ESP32-WROOM-32 dev board (COM4 / CP210x)
//   - USB connection for Serial Monitor
//   - Wi-Fi network with internet access
//
// Configuration:
//   Fill in WIFI_SSID, WIFI_PASSWORD, and DISCORD_WEBHOOK
//   below before flashing. Do NOT commit credentials to Git.
//
// Libraries Required (install via Arduino Library Manager):
//   - ArduinoJson  by Benoit Blanchon  >= 6.x
//     (HTTPClient and WiFi are bundled with the ESP32 core)
//
// Usage:
//   1. Fill in credentials in the CONFIG section below
//   2. Flash to board (Tools > Board: ESP32 Dev Module, Port: COM4)
//   3. Open Serial Monitor at 115200 baud
//   4. Observe output; confirm message in Discord channel
//   5. Board will enter deep sleep after test completes
// ============================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ── CONFIG ───────────────────────────────────────────────────
// Fill these in before flashing. Do NOT commit to Git.
const char* WIFI_SSID       = "WiFiName";
const char* WIFI_PASSWORD   = "YourWiFIPassword";
const char* DISCORD_WEBHOOK = "DiscordWebHook";

// Deep sleep duration after test completes (seconds)
// Set short for testing so board wakes quickly if needed
const int SLEEP_SECONDS = 30;

// Wi-Fi connection timeout (seconds)
const int WIFI_TIMEOUT_S = 10;
// ─────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  delay(2000);  // Allow Serial Monitor to connect

  Serial.println();
  Serial.println("============================================");
  Serial.println("  Discord Webhook Notification Test");
  Serial.println("  Solar Soil Moisture Monitor - Rev A");
  Serial.println("============================================");

  // ── STEP 1: Connect to Wi-Fi ─────────────────────────────
  Serial.println();
  Serial.print("[STEP 1] Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int elapsed = 0;
  while (WiFi.status() != WL_CONNECTED && elapsed < WIFI_TIMEOUT_S) {
    delay(1000);
    Serial.print(".");
    elapsed++;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[FAIL] Wi-Fi connection timed out after " + String(WIFI_TIMEOUT_S) + " s");
    Serial.println("       Check SSID, password, and network availability.");
    Serial.println("       Entering deep sleep to prevent retry loop.");
    goToSleep();
    return;
  }

  Serial.println("[PASS] Wi-Fi connected.");
  Serial.print("       IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.print("       Signal (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // ── STEP 2: Build Discord JSON Payload ───────────────────
  Serial.println();
  Serial.println("[STEP 2] Building Discord payload...");

  StaticJsonDocument<256> doc;
  doc["content"] = "✅ **Soil Moisture Monitor — Discord Test**\n"
                   "Webhook delivery confirmed.\n"
                   "ESP32 connected and HTTP POST successful.\n"
                   "_This is a test message from the firmware test suite._";

  String payload;
  serializeJson(doc, payload);

  Serial.println("         Payload: " + payload);

  // ── STEP 3: POST to Discord Webhook ──────────────────────
  Serial.println();
  Serial.println("[STEP 3] Sending HTTP POST to Discord webhook...");

  HTTPClient http;
  http.begin(DISCORD_WEBHOOK);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(payload);

  Serial.print("         HTTP Response Code: ");
  Serial.println(httpResponseCode);

  // Discord returns 204 No Content on success
  if (httpResponseCode == 204) {
    Serial.println("[PASS] HTTP 204 received — message delivered to Discord.");
    Serial.println("       Check your Discord channel to confirm the message.");
  } else if (httpResponseCode > 0) {
    Serial.println("[FAIL] Unexpected HTTP response code: " + String(httpResponseCode));
    Serial.println("       Check the webhook URL in the CONFIG section.");
    String responseBody = http.getString();
    if (responseBody.length() > 0) {
      Serial.println("       Response body: " + responseBody);
    }
  } else {
    Serial.println("[FAIL] HTTP POST failed. Error: " + http.errorToString(httpResponseCode));
    Serial.println("       Possible causes: no internet access, DNS failure, or invalid URL.");
  }

  http.end();

  // ── STEP 4: Disconnect Wi-Fi ─────────────────────────────
  Serial.println();
  Serial.println("[STEP 4] Disconnecting Wi-Fi...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("[DONE]   Wi-Fi disconnected.");

  // ── SUMMARY ──────────────────────────────────────────────
  Serial.println();
  Serial.println("============================================");
  Serial.println("  Test Complete");
  if (httpResponseCode == 204) {
    Serial.println("  Result: PASS");
  } else {
    Serial.println("  Result: FAIL — see details above");
  }
  Serial.println("============================================");

  goToSleep();
}

void loop() {
  // Not used — deep sleep prevents loop() from running
}

void goToSleep() {
  Serial.println();
  Serial.print("[SLEEP]  Entering deep sleep for ");
  Serial.print(SLEEP_SECONDS);
  Serial.println(" seconds...");
  Serial.println("         (Disconnect Serial Monitor before next flash)");
  Serial.flush();
  esp_deep_sleep(SLEEP_SECONDS * 1000000ULL);
}
