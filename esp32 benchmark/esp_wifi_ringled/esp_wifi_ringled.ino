#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi Credentials
const char* ssid = "admin";
const char* password = "12345678";

// LED Ring Setup
#define LED_PIN     13
#define LED_COUNT   16
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  // Initialize LED strip
  strip.begin();
  strip.show(); // Ensure all are off initially
  strip.setBrightness(50);

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

    // Light up LEDs
    colorWipe(strip.Color(0, 255, 0), 100); // Green for success
  } else {
    Serial.println("\nWiFi connection failed.");
    colorWipe(strip.Color(255, 0, 0), 100); // Red for failure
  }
}

void loop() {
  // Optional: Put your logic here
}

// Simple color wipe animation
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}
