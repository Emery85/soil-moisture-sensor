void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP32 Boot Test ===");
  Serial.println("Boot successful!");
  Serial.printf("CPU Freq: %d MHz\n", getCpuFrequencyMhz());
  Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
  Serial.println("=======================");

  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);
  Serial.println("LED ON");
  delay(500);
  digitalWrite(2, LOW);
  Serial.println("LED OFF");
  delay(500);
}