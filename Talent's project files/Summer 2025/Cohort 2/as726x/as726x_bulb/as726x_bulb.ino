#include <Wire.h>
#include <AS726X.h>

AS726X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!sensor.begin()) {
    Serial.println("Sensor not detected. Check wiring.");
    while (1); // Halt if sensor not found
  }

  // Turn ON indicator LED (small one)
//  sensor.enableIndicator();

  // Turn ON bulb LED (big one)
  sensor.enableBulb();

  Serial.println("Both LEDs are now ON.");
}

void loop() {
  // Do nothing
}
