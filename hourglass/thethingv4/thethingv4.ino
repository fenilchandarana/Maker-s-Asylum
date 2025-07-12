#include <MD_MAX72xx.h>
#include <SPI.h>

// Hardware type and number of devices
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 2

// Software SPI pin definitions
#define DATA_PIN 12  // DIN
#define CS_PIN   10  // CS
#define CLK_PIN  11  // CLK

// Initialize display with software SPI
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 1);
  mx.clear();

  // ==== Display 0: Square border ====
  uint8_t d0 = 0;  // left display
  for (uint8_t col = 0; col < 8; col++) {
    mx.setPoint(d0 * 8 + 0, col, true);   // Top row
    mx.setPoint(d0 * 8 + 7, col, true);   // Bottom row
  }
  for (uint8_t row = 1; row < 7; row++) {
    mx.setPoint(row, d0 * 8 + 0, true);   // Left column
    mx.setPoint(row, d0 * 8 + 7, true);   // Right column
  }

  // ==== Display 1: X ====
  uint8_t d1 = 1;  // right display
  for (uint8_t i = 0; i < 8; i++) {
    mx.setPoint(i, d1 * 8 + i, true);          // Top-left to bottom-right
    mx.setPoint(i, d1 * 8 + (7 - i), true);    // Top-right to bottom-left
  }
}

void loop() {
  // Static graphics, nothing here
}
