#include <MD_MAX72xx.h>
#include <SPI.h>

#define NUM_ROWS 16
#define NUM_COLS 8
#define LED_INTENSITY 1

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES (NUM_ROWS / 8)

#define DATA_PIN 12
#define CS_PIN   10
#define CLK_PIN  11

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ---- Just put your hex data here ----
const uint8_t top[8] = {
  0xFF, 0x81, 0x81, 0x81, 0x81, 0x42, 0x24, 0x18
};

const uint8_t bottom[8] = {
  0x18, 0x24, 0x42, 0x81, 0x81, 0x81, 0x81, 0xFF
};
// --------------------------------------

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  // Draw top part (rows 1–8)
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = top[r] & (1 << c);
      mx.setPoint(r % 8, c + (r / 8) * 8, on);
    }
  }

  // Draw bottom part (rows 9–16)
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t row = r + 8; // rows 9–16
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = bottom[r] & (1 << c);
      mx.setPoint(row % 8, c + (row / 8) * 8, on);
    }
  }
}

void loop() {
  // Static display — nothing here
}
