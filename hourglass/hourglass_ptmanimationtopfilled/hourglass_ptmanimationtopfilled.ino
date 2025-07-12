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

// Outline
const uint8_t outlineTop[8] = {
  0xFF, 0x81, 0x81, 0x81, 0x81, 0x42, 0x24, 0x18
};

const uint8_t outlineBottom[8] = {
  0x18, 0x24, 0x42, 0x81, 0x81, 0x81, 0x81, 0xFF
};

// Filled sand
const uint8_t filledTop[8] = {
  0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18
};

// Trickled version
const uint8_t trailTop[8] = {
0xFF,0x81,0xFF,0xFF,0xFF,0x7E,0x2C,0x18
};

const uint8_t trailBottom[8] = {
0x18,0x24,0x42,0x81,0x81,0x81,0x91,0xFF
};

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  // Draw filled sand top
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = filledTop[r] & (1 << c);
      mx.setPoint(r, c, on);
    }
  }
  // Draw outline bottom
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t row = r + 8;
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = outlineBottom[r] & (1 << c);
      mx.setPoint(row % 8, c + (row / 8) * 8, on);
    }
  }
}

void loop() {
  // === Find pixel turned OFF ===
  uint8_t srcRow = 0, srcCol = 0;
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t diff = filledTop[r] ^ trailTop[r];
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      if (diff & (1 << c)) {
        srcRow = r;
        srcCol = c;
      }
    }
  }

  // === Find where grain lands in bottom ===
  uint8_t dstRow = 0, dstCol = 0;
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t diff = trailBottom[r] ^ outlineBottom[r];
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      if (diff & (1 << c)) {
        dstRow = r + 8;
        dstCol = c;
      }
    }
  }

  // === 1️⃣ Turn OFF the pixel on top ===
  mx.setPoint(srcRow, srcCol, false);

  // === 2️⃣ Animate trickle on bottom ===
  for (uint8_t r = 8; r <= dstRow; r++) {
    uint8_t localRow = r - 8;
    bool isOutline = (outlineBottom[localRow] & (1 << dstCol));
    if (!isOutline) {
      mx.setPoint(r % 8, dstCol + (r / 8) * 8, true);
    }
    delay(100);
    if (r != dstRow && !isOutline) {
      mx.setPoint(r % 8, dstCol + (r / 8) * 8, false);
    }
  }

  delay(200);

  // === 3️⃣ Shift the sand down in top, respecting outline ===
  uint8_t newTop[8];
  memcpy(newTop, filledTop, 8);

  // Clear the dropped pixel first
  newTop[srcRow] &= ~(1 << srcCol);

  // Slide sand column down: only sand inside outline moves
  for (int8_t r = srcRow; r > 0; r--) {
    bool aboveIsSand = (filledTop[r - 1] & (1 << srcCol)) && !(outlineTop[r - 1] & (1 << srcCol));
    bool hereIsOutline = outlineTop[r] & (1 << srcCol);

    if (aboveIsSand && !hereIsOutline) {
      newTop[r] |= (1 << srcCol); // fill this row
    } else {
      newTop[r] &= ~(1 << srcCol); // clear this row
    }
  }

  // Top row always clears the sand only, outline stays
  newTop[0] &= ~(1 << srcCol);

  // OR to ensure outline never disappears:
  for (uint8_t r = 0; r < 8; r++) {
    newTop[r] |= (outlineTop[r] & (1 << srcCol)); // keep outline ON for this col
  }

  // === Draw whole new top ===
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = newTop[r] & (1 << c);
      mx.setPoint(r, c, on);
    }
  }

  while (true);
}




void animateTopShift(uint8_t startRow, uint8_t col) {
  // Slide pixels above the gap down by 1 step
  for (int8_t r = startRow; r > 0; r--) {
    // Copy pixel above down
    bool aboveOn = mx.getPoint(r - 1, col);
    mx.setPoint(r, col, aboveOn);
    delay(100);
  }
  // Top row cleared
  mx.setPoint(0, col, false);
}
