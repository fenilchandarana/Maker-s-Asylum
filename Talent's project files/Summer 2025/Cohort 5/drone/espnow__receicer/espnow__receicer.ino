#include <WiFi.h>
#include <esp_now.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("ESP2 MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_register_recv_cb([](const uint8_t *mac, const uint8_t *incomingData, int len) {
    Serial.print("Received: ");
    Serial.println((char *)incomingData);
  });
}

void loop() {
  // Nothing needed here
}
