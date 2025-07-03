#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Select your camera model
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Wi-Fi credentials
const char* ssid = "MuSo-Guest";
const char* password = "MuSo-JSW@360";

// Optional: Set static IP
IPAddress local_IP(192, 168, 0, 245);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Connect to Wi-Fi
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Setup endpoint to capture fresh photo each time
  server.on("/photo", HTTP_GET, [](AsyncWebServerRequest *request) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      request->send(500, "text/plain", "Camera capture failed");
      return;
    }
    request->send_P(200, "image/jpeg", fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  server.begin();
  Serial.print("Camera Ready! Visit: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/photo");
}

void loop() {
  // nothing here, everything handled by server
}
