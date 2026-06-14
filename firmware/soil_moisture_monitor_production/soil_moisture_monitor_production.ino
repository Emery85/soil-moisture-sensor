// ============================================================
// Soil Moisture Monitor — Production Firmware
// ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1)
// Toolchain: Arduino IDE (locked)
//
// Schedule: Wakes at 8:00 AM and 8:00 PM EDT
//           Takes soil moisture reading
//           Sends alert to Discord via webhook
//           Returns to deep sleep
//
// Hardware:
//   - Capacitive soil moisture sensor v1.2 on GPIO34 (ADC1)
//   - N-channel MOSFET gate on GPIO26 (sensor power switch)
//     * 330Ω series gate resistor
//     * 10kΩ pull-down to GND
//   - TP4056 + DW01A charging circuit
//   - 18650 3400mAh cell
//   - 6V 3W solar panel
//
// Calibration (from T-008 soil moisture test):
//   Dry air  ≈ 3225 (ADC raw)
//   Water    ≈ 1100 (ADC raw)
//
// Moisture Tiers (5-level):
//   0–19%   → DRY      🔴
//   20–39%  → LOW      🟠
//   40–59%  → MODERATE 🟡
//   60–79%  → MOIST    🟢
//   80–100% → WET      🔵
// ============================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"   // WiFi credentials + Discord webhook URL (not in Git)

// ── Pin Definitions ─────────────────────────────────────────
#define MOISTURE_PIN      34    // ADC1_CH6 — safe with WiFi active
#define MOSFET_GATE_PIN   26    // Sensor power via N-channel MOSFET

// ── Calibration Constants ────────────────────────────────────
#define ADC_DRY           3225  // Raw ADC reading in dry air
#define ADC_WET           1100  // Raw ADC reading submerged in water

// ── ADC Sampling ────────────────────────────────────────────
#define NUM_SAMPLES       10    // Averaged readings for stability

// ── Moisture Tier Thresholds (%) ────────────────────────────
#define TIER_DRY_MAX      19
#define TIER_LOW_MAX      39
#define TIER_MODERATE_MAX 59
#define TIER_MOIST_MAX    79
// 80–100 = WET

// ── Deep Sleep / Scheduling ──────────────────────────────────
// Target wakeup times expressed directly in UTC
// EDT = UTC-4, so: 8 AM EDT = 12:00 UTC, 8 PM EDT = 00:00 UTC
#define TARGET_HOUR_1_UTC  12   // 8:00 AM EDT
#define TARGET_HOUR_2_UTC  0    // 8:00 PM EDT
#define EDT_DISPLAY_OFFSET 4    // Subtract from UTC for display only

// NTP config
#define NTP_SERVER        "pool.ntp.org"
#define NTP_GMT_OFFSET    0          // We work in UTC, convert locally
#define NTP_DST_OFFSET    0

// ── WiFi Retry ───────────────────────────────────────────────
#define WIFI_TIMEOUT_MS   15000
#define WIFI_RETRY_COUNT  3

// ── Sensor Warmup ────────────────────────────────────────────
#define SENSOR_WARMUP_MS  500   // Allow sensor to stabilize after power-on

// ============================================================
// Moisture Tier Struct
// ============================================================
struct MoistureTier {
  const char* label;
  const char* emoji;
  const char* description;
};

MoistureTier getTier(int pct) {
  if (pct <= TIER_DRY_MAX)      return { "DRY",      "🔴", "Soil is very dry — water soon!" };
  if (pct <= TIER_LOW_MAX)      return { "LOW",       "🟠", "Soil moisture is low — consider watering." };
  if (pct <= TIER_MODERATE_MAX) return { "MODERATE",  "🟡", "Soil moisture is adequate." };
  if (pct <= TIER_MOIST_MAX)    return { "MOIST",     "🟢", "Soil is nicely moist." };
  return                               { "WET",       "🔵", "Soil is saturated — no watering needed." };
}

// ============================================================
// ADC → Moisture %
// ============================================================
int readMoisturePercent() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(MOISTURE_PIN);
    delay(10);
  }
  int raw = sum / NUM_SAMPLES;

  // Map: ADC_DRY (dry) → 0%, ADC_WET (wet) → 100%
  // Higher ADC = drier; invert the mapping.
  int pct = map(raw, ADC_DRY, ADC_WET, 0, 100);
  pct = constrain(pct, 0, 100);

  Serial.printf("[Sensor] Raw ADC avg: %d  →  Moisture: %d%%\n", raw, pct);
  return pct;
}

// ============================================================
// WiFi Connect
// ============================================================
bool connectWiFi() {
  Serial.printf("[WiFi] Connecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println("\n[WiFi] TIMEOUT — could not connect.");
      return false;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\n[WiFi] Connected. IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

// ============================================================
// NTP Time Fetch
// Returns current hour in EDT (UTC-4), or -1 on failure
// ============================================================
int getCurrentHourEDT() {
  configTime(NTP_GMT_OFFSET, NTP_DST_OFFSET, NTP_SERVER);
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10) {
    delay(500);
    attempts++;
  }
  if (attempts >= 10) {
    Serial.println("[NTP] Failed to get time.");
    return -1;
  }
  int hourUTC = timeinfo.tm_hour;
  int hourEDT = (hourUTC - EDT_DISPLAY_OFFSET + 24) % 24;
  Serial.printf("[NTP] UTC: %02d:%02d  →  EDT: %02d:%02d\n",
                timeinfo.tm_hour, timeinfo.tm_min, hourEDT, timeinfo.tm_min);
  return hourEDT;
}

// ============================================================
// Deep Sleep — calculate seconds until next wakeup window
// ============================================================
uint64_t secondsUntilNextWakeup() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // Fallback: sleep 6 hours and retry
    Serial.println("[Sleep] Time unavailable — defaulting to 6hr sleep.");
    return 6ULL * 3600ULL;
  }

  int hourUTC = timeinfo.tm_hour;
  int minUTC  = timeinfo.tm_min;
  int secUTC  = timeinfo.tm_sec;

  // Use UTC target hours directly — no offset arithmetic needed
  int currentTotalSec = hourUTC * 3600 + minUTC * 60 + secUTC;
  int target1Sec = TARGET_HOUR_1_UTC * 3600;  // 12:00 UTC = 8 AM EDT
  int target2Sec = TARGET_HOUR_2_UTC * 3600;  // 00:00 UTC = 8 PM EDT

  // Seconds remaining until each target today
  int diff1 = target1Sec - currentTotalSec;
  int diff2 = target2Sec - currentTotalSec;

  // Wrap negative values to "tomorrow"
  if (diff1 <= 60) diff1 += 86400;   // 60s grace — don't re-fire immediately
  if (diff2 <= 60) diff2 += 86400;

  int sleepSec = min(diff1, diff2);
  Serial.printf("[Sleep] Sleeping for %d seconds (~%.1f hours).\n",
                sleepSec, sleepSec / 3600.0);
  return (uint64_t)sleepSec;
}

// ============================================================
// Discord Webhook — send moisture reading
// ============================================================
bool sendDiscordAlert(int moisturePct, MoistureTier tier, int hourEDT) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Discord] Not connected to WiFi.");
    return false;
  }

  // Build timestamp string (AM/PM)
  const char* period = (hourEDT < 12) ? "AM" : "PM";
  int displayHour = hourEDT % 12;
  if (displayHour == 0) displayHour = 12;

  // Build JSON payload with Discord embed
  StaticJsonDocument<512> doc;
  JsonArray embeds = doc.createNestedArray("embeds");
  JsonObject embed  = embeds.createNestedObject();

  // Title
  char title[64];
  snprintf(title, sizeof(title), "%s %s  Soil Moisture — %d:00 %s EDT",
           tier.emoji, tier.label, displayHour, period);
  embed["title"] = title;

  // Description
  char desc[128];
  snprintf(desc, sizeof(desc), "%s\n\n**Moisture Level:** %d%%", tier.description, moisturePct);
  embed["description"] = desc;

  // Color per tier (Discord decimal color)
  // DRY=red, LOW=orange, MODERATE=yellow, MOIST=green, WET=blue
  uint32_t colors[] = { 0xE74C3C, 0xE67E22, 0xF1C40F, 0x2ECC71, 0x3498DB };
  int tierIndex = 0;
  if      (moisturePct <= TIER_DRY_MAX)      tierIndex = 0;
  else if (moisturePct <= TIER_LOW_MAX)      tierIndex = 1;
  else if (moisturePct <= TIER_MODERATE_MAX) tierIndex = 2;
  else if (moisturePct <= TIER_MOIST_MAX)    tierIndex = 3;
  else                                        tierIndex = 4;
  embed["color"] = colors[tierIndex];

  // Footer
  JsonObject footer = embed.createNestedObject("footer");
  footer["text"] = "Garden Monitor · Fredericksburg, VA";

  // Moisture bar (visual) in a field
  JsonArray fields = embed.createNestedArray("fields");
  JsonObject barField = fields.createNestedObject();
  barField["name"] = "Level";
  int filled = moisturePct / 10;
  char bar[32];
  int idx = 0;
  for (int i = 0; i < 10; i++) {
    bar[idx++] = (i < filled) ? '#' : '-';
  }
  bar[idx] = '\0';
  char barStr[48];
  snprintf(barStr, sizeof(barStr), "`[%s] %d%%`", bar, moisturePct);
  barField["value"]  = barStr;
  barField["inline"] = false;

  // Serialize
  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin(DISCORD_WEBHOOK_URL);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(payload);

  Serial.printf("[Discord] HTTP %d\n", httpCode);
  http.end();

  // 204 = success, 429 = rate limited
  return (httpCode == 204);
}

// ============================================================
// setup() — runs once per wakeup
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n========================================");
  Serial.println(" Soil Moisture Monitor — Wakeup");
  Serial.println("========================================");

  // ── 1. Power on sensor via MOSFET ───────────────────────
  pinMode(MOSFET_GATE_PIN, OUTPUT);
  digitalWrite(MOSFET_GATE_PIN, HIGH);
  Serial.println("[MOSFET] Sensor power ON.");
  delay(SENSOR_WARMUP_MS);

  // ── 2. Connect WiFi ─────────────────────────────────────
  bool wifiOK = false;
  for (int attempt = 1; attempt <= WIFI_RETRY_COUNT; attempt++) {
    Serial.printf("[WiFi] Attempt %d/%d\n", attempt, WIFI_RETRY_COUNT);
    wifiOK = connectWiFi();
    if (wifiOK) break;
    WiFi.disconnect(true);
    delay(2000);
  }

  if (!wifiOK) {
    Serial.println("[ERROR] WiFi failed — entering sleep for retry.");
    // Sleep 30 minutes and retry
    digitalWrite(MOSFET_GATE_PIN, LOW);
    esp_sleep_enable_timer_wakeup(30ULL * 60ULL * 1000000ULL);
    esp_deep_sleep_start();
  }

  // ── 3. Get current time (EDT) ───────────────────────────
  int hourEDT = getCurrentHourEDT();

  // ── 4. Read soil moisture ────────────────────────────────
  int moisturePct = readMoisturePercent();
  MoistureTier tier = getTier(moisturePct);
  Serial.printf("[Reading] %d%% — %s %s\n", moisturePct, tier.emoji, tier.label);

  // ── 5. Send Discord alert ────────────────────────────────
  bool sent = sendDiscordAlert(moisturePct, tier, hourEDT);
  if (sent) {
    Serial.println("[Discord] Alert sent successfully.");
  } else {
    Serial.println("[Discord] Alert failed (will retry next wakeup).");
  }

  // ── 6. Power off sensor ──────────────────────────────────
  digitalWrite(MOSFET_GATE_PIN, LOW);
  Serial.println("[MOSFET] Sensor power OFF.");

  // ── 7. Disconnect WiFi cleanly ───────────────────────────
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("[WiFi] Disconnected.");

  // ── 8. Calculate sleep duration and deep sleep ──────────
  uint64_t sleepSec = secondsUntilNextWakeup();
  uint64_t sleepUs  = sleepSec * 1000000ULL;

  Serial.println("[Sleep] Entering deep sleep. Goodnight.");
  Serial.println("========================================\n");
  delay(100);   // Flush serial buffer

  esp_sleep_enable_timer_wakeup(sleepUs);
  esp_deep_sleep_start();
}

// ============================================================
// loop() — never reached (deep sleep restarts setup())
// ============================================================
void loop() {
  // Intentionally empty
}
