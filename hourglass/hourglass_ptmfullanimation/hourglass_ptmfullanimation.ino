#include <MD_MAX72xx.h>
#include <SPI.h>
#include "animations.h"

#define NUM_ROWS 16
#define NUM_COLS 8
#define LED_INTENSITY 1

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES (NUM_ROWS / 8)

#define DATA_PIN 12
#define CS_PIN   10
#define CLK_PIN  11

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Static parts
const uint8_t outlineTop[8] = {
  0xFF, 0x81, 0x81, 0x81, 0x81, 0x42, 0x24, 0x18
};

const uint8_t outlineBottom[8] = {
  0x18, 0x24, 0x42, 0x81, 0x81, 0x81, 0x81, 0xFF
};

const uint8_t filledTop[8] = {
  0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18
};

// Grain struct for multiple grains
struct Grain {
  uint8_t dstRow;
  uint8_t dstCol;
  uint8_t currentRow;
  bool done;
};

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  // Draw initial filled top
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = filledTop[r] & (1 << c);
      mx.setPoint(r, c, on);
    }
  }

  // Draw static outline bottom
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t row = r + 8;
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = outlineBottom[r] & (1 << c);
      mx.setPoint(row % 8, c + (row / 8) * 8, on);
    }
  }
}

void loop() {
  // Run 1st animation
  runAnimation(trailTop1, trailBottom1, finalTop1, finalBottom1);
  delay(500);

  runAnimation(trailTop2, trailBottom2, finalTop2, finalBottom2);
  delay(500);

  runAnimation(trailTop3, trailBottom3, finalTop3, finalBottom3);
  delay(500);

  //runAnimation(trailTop4, trailBottom4, finalTop4);
  //delay(500);
  //
  //runAnimation(trailTop5, trailBottom5, finalTop5);
  //delay(500);
  //
  //runAnimation(trailTop6, trailBottom6, finalTop6);
  //delay(500);
  //
  //runAnimation(trailTop7, trailBottom7, finalTop7);
  //delay(500);
  //
  //runAnimation(trailTop8, trailBottom8, finalTop8);
  //delay(500);
  //
  //runAnimation(trailTop9, trailBottom9, finalTop9);
  //delay(500);
  //
  //runAnimation(trailTop10, trailBottom10, finalTop10);
  //delay(500);
  //
  //runAnimation(trailTop11, trailBottom11, finalTop11);
  //delay(500);
  //
  //runAnimation(trailTop12, trailBottom12, finalTop12);
  //delay(500);
  //
  //runAnimation(trailTop13, trailBottom13, finalTop13);
  //delay(500);
  //
  //runAnimation(trailTop14, trailBottom14, finalTop14);
  //delay(500);
  //
  //runAnimation(trailTop15, trailBottom15, finalTop15);
  //delay(500);
  //
  //runAnimation(trailTop16, trailBottom16, finalTop16);
  //delay(500);
  //
  //runAnimation(trailTop17, trailBottom17, finalTop17);
  //delay(500);
  //
  //runAnimation(trailTop18, trailBottom18, finalTop18);
  //delay(500);
  //
  //runAnimation(trailTop19, trailBottom19, finalTop19);
  //delay(500);
  //
  //runAnimation(trailTop20, trailBottom20, finalTop20);
  //delay(500);
  //
  //runAnimation(trailTop21, trailBottom21, finalTop21);
  //delay(500);
  //
  //runAnimation(trailTop22, trailBottom22, finalTop22);
  //delay(500);
  //
  //runAnimation(trailTop23, trailBottom23, finalTop23);
  //delay(500);
  //
  //runAnimation(trailTop24, trailBottom24, finalTop24);
  //delay(500);


  while (true); // Stop repeating
}

void runAnimation(const uint8_t* trailTop, const uint8_t* trailBottom, const uint8_t* finalTop, const uint8_t* finalBottom) {
  // === Find trickle source ===
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

  // === Find trickle destination ===
  uint8_t dstRow = 255, dstCol = 255;

  // 1) Try to find diff pixel in SAME COLUMN
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t diff = trailBottom[r] ^ outlineBottom[r];
    if (diff & (1 << srcCol)) {
      dstRow = r + 8;
      dstCol = srcCol;
      break;
    }
  }

  // 2) Fallback: pick first new pixel anywhere
  if (dstRow == 255) {
    for (uint8_t r = 0; r < 8; r++) {
      uint8_t diff = trailBottom[r] ^ outlineBottom[r];
      for (uint8_t c = 0; c < NUM_COLS; c++) {
        if (diff & (1 << c)) {
          dstRow = r + 8;
          dstCol = c;
          goto DoneFinding;
        }
      }
    }
  }

DoneFinding:

  if (dstRow == 255 || dstCol == 255) {
    Serial.println("No trickle destination found!");
    return;
  }

  // === Prepare animation ===
  mx.setPoint(srcRow, srcCol, false);

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

    // Trickling down
    if (!trickleDone && now - lastTrickleTime >= 200) {
      uint8_t localRow = trickleRow - 8;
      bool isOutline = (outlineBottom[localRow] & (1 << dstCol));
      if (!isOutline) {
        mx.setPoint(trickleRow % 8, dstCol + (trickleRow / 8) * 8, true);
      }

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

    // Slide top down
    if (!slideStarted && now - startTime >= 200) slideStarted = true;

    if (slideStarted && !slideDone && now - lastSlideTime >= 200) {
      if (slideRow > 0) {
        if (!(outlineTop[slideRow] & (1 << srcCol)) && !(outlineTop[slideRow - 1] & (1 << srcCol))) {
          bool aboveOn = tempTop[slideRow - 1] & (1 << srcCol);
          if (aboveOn) {
            tempTop[slideRow] |= (1 << srcCol);
            tempTop[slideRow - 1] &= ~(1 << srcCol);
          }
        }

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

  // === Final fix for top ===
  for (uint8_t r = 0; r < 8; r++) {
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = finalTop[r] & (1 << c);
      mx.setPoint(r, c, on);
    }
  }

  // === Final fix for bottom ===
  for (uint8_t r = 0; r < 8; r++) {
    uint8_t row = r + 8;
    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool on = finalBottom[r] & (1 << c);
      mx.setPoint(row % 8, c + (row / 8) * 8, on);
    }
  }
}
