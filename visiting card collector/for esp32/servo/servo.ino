#include <ESP32Servo.h>

Servo myServo;  // Create a servo object
int servoPin = 22; // GPIO pin connected to the servo

void setup() {
  Serial.begin(9600);
//  myServo.setPeriodHertz(50);    // Standard 50 hz servo
  myServo.attach(servoPin);      // Attach the servo to a pin
}

void loop() {
  myServo.write(0);   // Move servo to 0 degrees
  delay(5000);
  myServo.write(170);  // Move servo to 90 degrees
  delay(5000);
}
