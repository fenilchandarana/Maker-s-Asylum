#include <MD_MAX72xx.h>
#include <SPI.h>
#include "patterns.h"

// ======= USER SETTINGS =======
#define NUM_ROWS 16
#define NUM_COLS 8
#define LED_INTENSITY 1  // 0-15

#define FLIP_BITS 1  // 0 = normal, 1 = flip

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES (NUM_ROWS / 8)

#define DATA_PIN 12
#define CS_PIN   10
#define CLK_PIN  11

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

uint8_t frame[NUM_ROWS];

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  for (int i = 0; i < NUM_ROWS; i++) frame[i] = 0;

  for (uint8_t i = 0; i < patternCount; i++) {
    processPattern(patternList[i]);
  }

  Serial.begin(9600);
  Serial.println("Pattern loaded from header file.");
}

void loop() {
  // Nothing — static pattern only
}

void processPattern(const char* input) {
  String line = String(input);
  line.trim();
  int dashIndex = line.indexOf('-');
  if (dashIndex == -1) return;

  String rowStr = line.substring(0, dashIndex);
  String patternStr = line.substring(dashIndex + 1);

  int userRow = rowStr.toInt();
  if (userRow < 1 || userRow > NUM_ROWS) return;
  if (patternStr.length() != NUM_COLS) return;

  uint8_t pattern = strtoul(patternStr.c_str(), NULL, 2);
  if (FLIP_BITS) pattern = flipBits(pattern);

  uint8_t globalRow = userRow - 1;
  frame[globalRow] = pattern;

  uint8_t module = globalRow / 8;
  uint8_t localRow = globalRow % 8;

  for (uint8_t col = 0; col < NUM_COLS; col++) {
    bool state = bitRead(pattern, NUM_COLS - 1 - col);
    mx.setPoint(localRow, col + module * NUM_COLS, state);
  }
}

uint8_t flipBits(uint8_t b) {
  uint8_t rev = 0;
  for (uint8_t i = 0; i < NUM_COLS; i++) {
    rev <<= 1;
    rev |= (b & 1);
    b >>= 1;
  }
  return rev;
}
