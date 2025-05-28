#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "time.h"

// Wi-Fi Credentials
const char* ssid = "admin";
const char* password = "12345678";  

// LED Ring Setup
#define LED_PIN     13
#define LED_COUNT   16
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Time server config
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // 5.5 hours × 3600 seconds
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(9600);

  // Initialize LED strip
  strip.begin();
  strip.show();
  strip.setBrightness(16);

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Set up NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(2000); // Give NTP time to sync

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      Serial.println("Time fetched from internet:");
      Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
      colorWipe(strip.Color(0, 255, 0), 100); // Green
    } else {
      Serial.println("Failed to get time from NTP.");
      colorWipe(strip.Color(255, 165, 0), 100); // Orange
    }

  } else {
    Serial.println("\nWiFi connection failed.");
    colorWipe(strip.Color(255, 0, 0), 100); // Red
  }
}

void loop() {
  // Nothing here for now
}

// Simple color wipe animation
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}
