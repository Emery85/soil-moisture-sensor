// ============================================================
// TEST: Capacitive Soil Moisture Sensor
// Project: Solar ESP32 Soil Monitor
// File:    firmware/tests/capacitor_sensor_test/capacitor_sensor_test.ino
// Board:   ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1)
// IDE:     Arduino IDE (PlatformIO incompatible with this chip rev)
//
// Purpose: Validate raw ADC readings from the capacitive sensor,
//          map to a 0–100% moisture percentage, and print results
//          to Serial Monitor. No WiFi, no cloud — standalone test.
//
// Wiring:
//   Sensor VCC  → 3.3V (or MOSFET-switched line when integrated)
//   Sensor GND  → GND
//   Sensor AOUT → GPIO34 (ADC1_CH6 — input only, safe for ADC)
//
// Calibration:
//   AIR_VALUE  : Reading with sensor fully in dry air
//   WATER_VALUE: Reading with sensor submerged in water
//   Adjust these after running the raw output section below.
// ============================================================

// --- Pin Definition ---
#define SOIL_PIN 34          // ADC1 channel — do NOT use ADC2 pins (conflict with WiFi)

// --- Calibration Constants (adjust after raw calibration run) ---
// Defaults are typical for common capacitive v1.2 modules at 3.3V.
// Run this sketch first with CALIBRATION_MODE true to get your values.
const int AIR_VALUE   = 3225;  // Raw ADC reading in dry air (sensor dry)
const int WATER_VALUE = 1100;  // Raw ADC reading submerged in water

// --- Calibration Mode ---
// Set to true for first run: prints raw ADC only so you can determine
// your AIR_VALUE and WATER_VALUE. Set to false for normal operation.
const bool CALIBRATION_MODE = false;

// --- Sampling Config ---
const int SAMPLE_COUNT    = 10;    // Readings to average per measurement
const int SAMPLE_DELAY_MS = 10;    // Delay between samples (ms)
const int READ_INTERVAL   = 2000;  // Interval between full readings (ms)

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);  // Allow Serial to stabilize

  Serial.println("============================================");
  Serial.println("  Soil Moisture Sensor Test");
  Serial.println("  ESP32 Solar Monitor Project");
  Serial.println("============================================");

  if (CALIBRATION_MODE) {
    Serial.println("MODE: CALIBRATION — raw ADC values only.");
    Serial.println("1. Hold sensor in AIR  → note the reading.");
    Serial.println("2. Submerge in WATER   → note the reading.");
    Serial.println("3. Update AIR_VALUE / WATER_VALUE, set");
    Serial.println("   CALIBRATION_MODE = false, re-flash.");
  } else {
    Serial.println("MODE: NORMAL OPERATION");
    Serial.print("  AIR_VALUE   = "); Serial.println(AIR_VALUE);
    Serial.print("  WATER_VALUE = "); Serial.println(WATER_VALUE);
  }
  Serial.println("--------------------------------------------");
}

// ============================================================
// HELPERS
// ============================================================

// Average multiple ADC readings to reduce noise
int readSoilRaw() {
  long total = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    total += analogRead(SOIL_PIN);
    delay(SAMPLE_DELAY_MS);
  }
  return (int)(total / SAMPLE_COUNT);
}

// Map raw ADC to 0–100% moisture, clamped to valid range
int rawToPercent(int raw) {
  // Capacitive sensors read HIGH in dry conditions, LOW when wet
  // So moisture % increases as raw value decreases
  int moisture = map(raw, AIR_VALUE, WATER_VALUE, 0, 100);
  return constrain(moisture, 0, 100);
}

// Human-readable moisture category
String moistureLabel(int pct) {
  if (pct < 20) return "DRY";
  if (pct < 40) return "LOW";
  if (pct < 60) return "MODERATE";
  if (pct < 80) return "MOIST";
  return "WET";
}

// ============================================================
// MAIN LOOP
// ============================================================
void loop() {
  int rawValue = readSoilRaw();

  if (CALIBRATION_MODE) {
    Serial.print("Raw ADC: ");
    Serial.println(rawValue);
  } else {
    int moisturePct = rawToPercent(rawValue);
    String label    = moistureLabel(moisturePct);

    Serial.print("Raw ADC: ");
    Serial.print(rawValue);
    Serial.print("  |  Moisture: ");
    Serial.print(moisturePct);
    Serial.print("%  |  Status: ");
    Serial.println(label);

    // Warn if raw reading is out of expected calibration range
    if (rawValue > AIR_VALUE + 200) {
      Serial.println("  [WARN] Raw reading above AIR_VALUE — check wiring or re-calibrate.");
    }
    if (rawValue < WATER_VALUE - 200) {
      Serial.println("  [WARN] Raw reading below WATER_VALUE — sensor may be saturated or shorted.");
    }
  }

  delay(READ_INTERVAL);
}
