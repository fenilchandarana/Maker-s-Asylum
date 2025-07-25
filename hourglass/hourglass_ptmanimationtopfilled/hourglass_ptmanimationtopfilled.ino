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
  0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0x7E, 0x34, 0x18
};

const uint8_t trailBottom[8] = {
  0x18, 0x24, 0x42, 0x81, 0x81, 0x81, 0x89, 0xFF
};

const uint8_t finalTop[8] = {
  0xFF, 0x81, 0xF7, 0xFF, 0xFF, 0x7E, 0x3C, 0x18
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
  // === Find trickle points ===
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

  mx.setPoint(srcRow, srcCol, false);

  // === Non-blocking loop ===
  uint8_t trickleRow = 8;
  uint8_t slideRow = srcRow;

  uint32_t lastTrickleTime = millis();
  uint32_t lastSlideTime = millis();
  uint32_t startTime = millis();
  bool slideStarted = false;

  bool trickleDone = false;
  bool slideDone = false;

  uint8_t tempTop[8];
  memcpy(tempTop, trailTop, 8);

  while (!trickleDone || !slideDone) {
    uint32_t now = millis();

    // Bottom trickle step
    if (!trickleDone && now - lastTrickleTime >= 200) {
      uint8_t localRow = trickleRow - 8;
      bool isOutline = (outlineBottom[localRow] & (1 << dstCol));
// Turn ON the current trickle pixel
if (!isOutline) {
  mx.setPoint(trickleRow % 8, dstCol + (trickleRow / 8) * 8, true);
}

// Turn OFF the previous pixel (only if not first)
if (trickleRow > 8) {
  uint8_t prevRow = trickleRow - 1;
  uint8_t localPrevRow = prevRow - 8;
  bool prevIsOutline = outlineBottom[localPrevRow] & (1 << dstCol);
  if (!prevIsOutline) {
    mx.setPoint(prevRow % 8, dstCol + (prevRow / 8) * 8, false);
  }
}
      trickleRow++;
      lastTrickleTime = now;
      if (trickleRow > dstRow) trickleDone = true;
    }

    // Start slide after 200ms
    if (!slideStarted && now - startTime >= 300) {
      slideStarted = true;
    }

    // Slide step
    if (slideStarted && !slideDone && now - lastSlideTime >= 200) {
      if (slideRow > 0) {
        if (!(outlineTop[slideRow] & (1 << srcCol)) && !(outlineTop[slideRow - 1] & (1 << srcCol))) {
          bool aboveOn = tempTop[slideRow - 1] & (1 << srcCol);
          if (aboveOn) {
            tempTop[slideRow] |= (1 << srcCol);
            tempTop[slideRow - 1] &= ~(1 << srcCol);
          }
        }
        // Draw
        for (uint8_t r = 0; r < 8; r++) {
          for (uint8_t c = 0; c < NUM_COLS; c++) {
            bool on = tempTop[r] & (1 << c);
            mx.setPoint(r, c, on);
          }
        }
        slideRow--;
        lastSlideTime = now;
      } else {
        slideDone = true;
      }
    }
  }

  // Final fix to match finalTop exactly
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = finalTop[r] & (1 << c);
      mx.setPoint(r, c, on);
    }
  }

  while (true);
}
