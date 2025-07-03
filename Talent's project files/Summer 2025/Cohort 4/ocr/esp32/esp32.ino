void setup() {
  Serial.begin(115200);
  Serial.println("ESP ready to echo...");
}

void loop() {
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');
    Serial.print("Echo: ");
    Serial.println(received);  // Echo back to Python
  }
}
