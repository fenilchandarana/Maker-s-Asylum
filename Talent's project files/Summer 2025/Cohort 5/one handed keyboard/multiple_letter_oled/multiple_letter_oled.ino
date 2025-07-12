#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int piezoPins[5] = {1, 9, 10, 6, 12};
const char* piezoLetters[5] = {
  "abcd",   // piezo 1
  "efgh",   // piezo 9
  "ijklmn", // piezo 10
  "opqrst", // piezo 6
  "uvwxyz"  // piezo 12
};

int previousState[5] = {0, 0, 0, 0, 0};
unsigned long lastTriggerTime[5] = {0, 0, 0, 0, 0};
const unsigned long debounceDelay = 100; // Debounce time
const unsigned long tapTimeout = 500;    // Multi-tap window

int tapCount[5] = {0, 0, 0, 0, 0};

// Store final text buffer
String typedText = "";

// Track active piezo
int activePiezo = -1;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 5; i++) {
    pinMode(piezoPins[i], INPUT);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void loop() {
  unsigned long currentTime = millis();

  for (int i = 0; i < 5; i++) {
    int currentState = digitalRead(piezoPins[i]);

    if (currentState == HIGH && previousState[i] == LOW) {
      if (currentTime - lastTriggerTime[i] > debounceDelay) {

        if (activePiezo == i) {
          // Same piezo → cycle letter
          tapCount[i]++;
          updateLastLetter(i);
        } else {
          // Different piezo → lock previous piezo
          if (activePiezo != -1) {
            tapCount[activePiezo] = 0;
          }
          // New piezo → add first letter
          activePiezo = i;
          tapCount[i] = 1;
          typedText += getLetter(i, tapCount[i]);
        }

        updateOLED();
        lastTriggerTime[i] = currentTime;
      }
    }

    previousState[i] = currentState;
  }

  // If pause time expired, lock active piezo
  if (activePiezo != -1 && (millis() - lastTriggerTime[activePiezo] > tapTimeout)) {
    tapCount[activePiezo] = 0;
    activePiezo = -1;
  }

  delay(1);
}

char getLetter(int piezoIndex, int count) {
  const char* letters = piezoLetters[piezoIndex];
  int numLetters = strlen(letters);
  int index = (count - 1) % numLetters;
  return letters[index];
}

void updateLastLetter(int piezoIndex) {
  if (typedText.length() > 0) {
    // Remove last letter
    typedText.remove(typedText.length() - 1);
  }
  // Append new letter
  typedText += getLetter(piezoIndex, tapCount[piezoIndex]);
}

void updateOLED() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(typedText);
  display.display();
}
