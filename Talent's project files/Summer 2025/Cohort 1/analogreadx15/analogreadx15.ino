const int piezoPins[15] = {32, 33, 34, 35, 36, 39, 25, 26, 27, 14, 12, 13, 2, 4, 15};

void setup() {
  Serial.begin(115200);
  // Print headers once for Serial Plotter
  for (int i = 0; i < 15; i++) {
    Serial.print("Piezo");
    Serial.print(i + 1);
    if (i < 14) Serial.print(", ");
  }
  Serial.println();
}

void loop() {
  for (int i = 0; i < 15; i++) {
    int value = analogRead(piezoPins[i]);
    Serial.print(value);
    if (i < 14) Serial.print(", ");
  }
  Serial.println(); // Next data line
  delay(50); // Adjust for responsiveness
}
