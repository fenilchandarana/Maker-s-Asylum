#include <MD_MAX72xx.h>
#include <SPI.h>

// ======= USER SETTINGS =======
#define NUM_ROWS 16
#define NUM_COLS 8
#define LED_INTENSITY 2  // 0-15

// Flip the bit order? (0 = normal, 1 = flip)
#define FLIP_BITS 1  // <-- set to 1 to flip orientation  

// =============================

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES (NUM_ROWS / 8)

#define DATA_PIN 12
#define CS_PIN   10
#define CLK_PIN  11

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

String inputString = "";

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, LED_INTENSITY);
  mx.clear();

  Serial.begin(9600);
  Serial.println("Enter command: ROW-PATTERN (e.g., 5-11111110)");
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      processInput(inputString);
      inputString = "";
    } else if (inChar != '\r') {
      inputString += inChar;
    }
  }
}

void processInput(String input) {
  input.trim();
  int dashIndex = input.indexOf('-');
  if (dashIndex == -1) {
    Serial.println("Invalid format. Use: ROW-PATTERN");
    return;
  }

  String rowStr = input.substring(0, dashIndex);
  String patternStr = input.substring(dashIndex + 1);

  int userRow = rowStr.toInt();
  if (userRow < 1 || userRow > NUM_ROWS) {
    Serial.print("Row must be 1–");
    Serial.println(NUM_ROWS);
    return;
  }

  if (patternStr.length() != NUM_COLS) {
    Serial.print("Pattern must be ");
    Serial.print(NUM_COLS);
    Serial.println(" bits.");
    return;
  }

  uint8_t pattern = strtoul(patternStr.c_str(), NULL, 2);

  if (FLIP_BITS) {
    pattern = flipBits(pattern);
  }

  uint8_t globalRow = userRow - 1;
  uint8_t module = globalRow / 8;
  uint8_t localRow = globalRow % 8;

  for (uint8_t col = 0; col < NUM_COLS; col++) {
    bool state = bitRead(pattern, NUM_COLS - 1 - col);
    mx.setPoint(localRow, col + module * NUM_COLS, state);
  }

  Serial.print("Updated row ");
  Serial.print(userRow);
  Serial.print(" with pattern ");
  Serial.println(patternStr);
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
