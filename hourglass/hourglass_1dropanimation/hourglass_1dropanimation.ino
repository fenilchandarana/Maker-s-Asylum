#include <MD_MAX72xx.h>
#include <SPI.h>
#include "patterns.h"

#define NUM_ROWS 16
#define NUM_COLS 8
#define LED_INTENSITY 3

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES (NUM_ROWS / 8)

#define DATA_PIN 12
#define CS_PIN   10
#define CLK_PIN  11

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  // Draw static outline ONCE
  for (uint8_t r = 0; r < 16; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = outline[r] & (1 << c);
      mx.setPoint(r % 8, c + (r / 8) * 8, on);
    }
  }

  delay(500);

  // Animate 1 grain with safe overlap
  int8_t col = 3;  // column for grain, adjust for funnel

  int8_t prevRow = -1;
  for (uint8_t y = 2; y <= 14; y++) {
    if (prevRow >= 0) {
      bool wasOutline = outline[prevRow] & (1 << col);
      mx.setPoint(prevRow % 8, col + (prevRow / 8) * 8, wasOutline);
    }

    mx.setPoint(y % 8, col + (y / 8) * 8, true);

    prevRow = y;
    delay(150);
  }

  // Leave final grain ON
}

void loop() {
  // nothing
}
