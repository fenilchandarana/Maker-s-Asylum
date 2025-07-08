#include <WiFi.h>
#include <esp_now.h>

#define BUTTON1_PIN 18
#define BUTTON2_PIN 19

uint8_t receiverMac[] = {0x00, 0x4B, 0x12, 0xEE, 0xB7, 0x24}; 

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  Serial.print("ESP1 MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_add_peer(&peerInfo)) {
    Serial.println("Peer added.");
  }

  esp_now_register_send_cb([](const uint8_t *mac, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  });
}

void loop() {
  if (digitalRead(BUTTON1_PIN) == LOW) {
    const char *msg = "Button 1 pressed";
    esp_now_send(receiverMac, (uint8_t *)msg, strlen(msg) + 1);
    delay(500);
  }

  if (digitalRead(BUTTON2_PIN) == LOW) {
    const char *msg = "Button 2 pressed";
    esp_now_send(receiverMac, (uint8_t *)msg, strlen(msg) + 1);
    delay(500);
  }
}
