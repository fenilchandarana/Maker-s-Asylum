#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

// === USER CONFIGURATION ===
const int SERVO_PIN = 13;           // Servo signal pin (D23)
const int CLOCKWISE_ANGLE = 180;    // Target angle for clockwise movement
const int ANTICLOCKWISE_ANGLE = 0;  // Target angle for anticlockwise movement
const int STEP_DELAY_MS = 10;       // Delay between steps (lower = faster)
const int STEP_SIZE = 1;            // Step increment per loop (higher = faster)

// Button commands (can be customized)
const char* BUTTON1_COMMAND = "Button 1 pressed";
const char* BUTTON2_COMMAND = "Button 2 pressed";

// === GLOBALS ===
Servo myServo;
int currentAngle = 90;  // Initial angle (neutral)
int targetAngle = 90;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("ESP2 MAC: ");
  Serial.println(WiFi.macAddress());

  myServo.attach(SERVO_PIN);
  myServo.write(currentAngle); // Move to initial position

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);
}

void loop() {
  // Smooth transition to target angle
  if (currentAngle != targetAngle) {
    if (currentAngle < targetAngle)
      currentAngle += STEP_SIZE;
    else if (currentAngle > targetAngle)
      currentAngle -= STEP_SIZE;

    myServo.write(currentAngle);
    delay(STEP_DELAY_MS);
  }
}

void onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  String msg = String((char*)incomingData);
  Serial.print("Received: ");
  Serial.println(msg);

  if (msg == BUTTON1_COMMAND) {
    targetAngle = CLOCKWISE_ANGLE;
    Serial.println("→ Rotating Clockwise");
  } else if (msg == BUTTON2_COMMAND) {
    targetAngle = ANTICLOCKWISE_ANGLE;
    Serial.println("← Rotating Anticlockwise");
  }
}
