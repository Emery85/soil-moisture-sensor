#define MOSFET_PIN 2
#define BLINK_INTERVAL 1000  // ms

void setup() {
  Serial.begin(115200);
  pinMode(MOSFET_PIN, OUTPUT);
  Serial.println("T-004 / T-005 MOSFET blink test started");
}

void loop() {
  // T-004: Gate HIGH — MOSFET ON, LED should light
  digitalWrite(MOSFET_PIN, HIGH);
  Serial.println("GATE HIGH — LED should be ON");
  delay(BLINK_INTERVAL);

  // T-005: Gate LOW — MOSFET OFF, LED should be dark
  digitalWrite(MOSFET_PIN, LOW);
  Serial.println("GATE LOW  — LED should be OFF");
  delay(BLINK_INTERVAL);
}