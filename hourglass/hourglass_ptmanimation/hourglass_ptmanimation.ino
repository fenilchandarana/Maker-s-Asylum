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

// ---- Your static outline ----
const uint8_t outlineTop[8] = {
  0xFF, 0x81, 0x81, 0x81, 0x81, 0x42, 0x24, 0x18
};

const uint8_t outlineBottom[8] = {
  0x18, 0x24, 0x42, 0x81, 0x81, 0x81, 0x81, 0xFF
};

// ---- Your trail hex ----
const uint8_t trailTop[8] = {
  0xFF, 0x81, 0x91, 0x91, 0x91, 0x52, 0x34, 0x18
};

const uint8_t trailBottom[8] = {
  0x18, 0x34, 0x52, 0x91, 0x91, 0x91, 0x91, 0xFF
};

// ---- Store trail points ----
struct Point {
  uint8_t row;
  uint8_t col;
};

Point trailPoints[128];
uint16_t trailLength = 0;

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  // Draw outline once
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = outlineTop[r] & (1 << c);
      mx.setPoint(r, c, on);
    }
  }
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t row = r + 8;
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = outlineBottom[r] & (1 << c);
      mx.setPoint(row % 8, c + (row / 8) * 8, on);
    }
  }

  // Compute trail points (diff bits)
  trailLength = 0;
  // Top 8 rows
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t diff = trailTop[r] ^ outlineTop[r];
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      if (diff & (1 << c)) {
        trailPoints[trailLength++] = { r, c };
      }
    }
  }
  // Bottom 8 rows (rows 8–15)
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t diff = trailBottom[r] ^ outlineBottom[r];
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      if (diff & (1 << c)) {
        trailPoints[trailLength++] = { r + 8, c };
      }
    }
  }
}

void loop() {
  static int16_t prevRow = -1;
  static int16_t prevCol = -1;

  for (uint16_t i = 0; i < trailLength; i++) {
    Point p = trailPoints[i];
    uint8_t row = p.row;
    uint8_t col = p.col;

    // Turn OFF previous dot, restore outline if needed
    if (prevRow >= 0 && prevCol >= 0) {
      bool wasOutline;
      if (prevRow < 8) {
        wasOutline = outlineTop[prevRow] & (1 << prevCol);
      } else {
        wasOutline = outlineBottom[prevRow - 8] & (1 << prevCol);
      }
      mx.setPoint(prevRow % 8, prevCol + (prevRow / 8) * 8, wasOutline);
    }

    // Turn ON current dot
    mx.setPoint(row % 8, col + (row / 8) * 8, true);

    prevRow = row;
    prevCol = col;

    delay(150);
  }

  delay(500);
}
