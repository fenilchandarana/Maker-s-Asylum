#include <ESP32Servo.h>

Servo myServo;  
int servoPin = 22;    // Servo connected to GPIO22
int pushButton = 19;  // Input pin (D19)

void setup() {
  Serial.begin(9600);
  pinMode(pushButton, INPUT);   // Button input
  myServo.attach(servoPin);     // Attach servo
  myServo.write(0);             // Step 1: Move to 0° on startup
  Serial.println("Startup: Servo at 0°");
}

void loop() {
  int buttonState = digitalRead(pushButton);
  Serial.println(buttonState);   // print 0 or 1

  if (buttonState == 0) {       // Step 2: If digitalRead is 0
    delay(200);                 // wait 500ms
    myServo.write(120);         // move to 120°
    Serial.println("Servo at 120°");
    delay(5000);                // Step 3: wait 5000ms

    // Step 4: Gradually return to 0
    for (int pos = 120; pos >= 0; pos -= 15) {
      myServo.write(pos);
      Serial.print("Servo at ");
      Serial.print(pos);
      Serial.println("°");
      delay(500);
    }
  }
}
