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
const unsigned long debounceDelay = 100;
const unsigned long tapTimeout = 500;
const unsigned long comboWindow = 100;

int tapCount[5] = {0, 0, 0, 0, 0};

String typedText = "";
int activePiezo = -1;
bool capsLock = false;

bool pressedThisCycle[5] = {false, false, false, false, false};
bool comboActive = false;
unsigned long comboStartTime = 0;

// === Combo definition ===

void doBackspace();
void toggleCapsLock();
void addSpace();
void addNewline();

struct Combo {
  bool keys[5];
  int count;
  void (*action)();
};

Combo combos[] = {
  // {1,9,10,6,12} order
  {{false, true, true, false, false}, 2, doBackspace},
  {{false, true, true, true, false}, 3, toggleCapsLock},
  {{false, true, true, true, true}, 4, addSpace},
  {{true, true, true, true, true}, 5, addNewline}
};

const int numCombos = sizeof(combos) / sizeof(combos[0]);

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

        pressedThisCycle[i] = true;

        if (!comboActive) {
          comboActive = true;
          comboStartTime = currentTime;
        }

        lastTriggerTime[i] = currentTime;
      }
    }

    previousState[i] = currentState;
  }

  if (comboActive && (currentTime - comboStartTime > comboWindow)) {
    handleComboOrLetters();
    for (int i = 0; i < 5; i++) pressedThisCycle[i] = false;
    comboActive = false;
  }

  if (activePiezo != -1 && (currentTime - lastTriggerTime[activePiezo] > tapTimeout)) {
    tapCount[activePiezo] = 0;
    activePiezo = -1;
  }

  delay(1);
}

void handleComboOrLetters() {
  int count = 0;
  bool comboFlags[5];
  for (int i = 0; i < 5; i++) {
    comboFlags[i] = pressedThisCycle[i];
    if (comboFlags[i]) count++;
  }

  bool matched = false;

  for (int c = 0; c < numCombos; c++) {
    bool match = true;
    for (int i = 0; i < 5; i++) {
      if (combos[c].keys[i] != comboFlags[i]) {
        match = false;
        break;
      }
    }
    if (match && count == combos[c].count) {
      combos[c].action();
      matched = true;
      break;
    }
  }

  if (!matched) {
    for (int i = 0; i < 5; i++) {
      if (pressedThisCycle[i]) {
        if (activePiezo == i) {
          tapCount[i]++;
          updateLastLetter(i);
        } else {
          if (activePiezo != -1) tapCount[activePiezo] = 0;
          activePiezo = i;
          tapCount[i] = 1;
          typedText += getLetter(i, tapCount[i]);
        }
      }
    }
  }

  updateOLED();
}

char getLetter(int piezoIndex, int count) {
  const char* letters = piezoLetters[piezoIndex];
  int numLetters = strlen(letters);
  int index = (count - 1) % numLetters;
  char letter = letters[index];
  if (capsLock) {
    letter = toupper(letter);
  }
  return letter;
}

void updateLastLetter(int piezoIndex) {
  if (typedText.length() > 0) {
    typedText.remove(typedText.length() - 1);
  }
  typedText += getLetter(piezoIndex, tapCount[piezoIndex]);
}

void updateOLED() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(typedText);
  display.display();
}

void doBackspace() {
  if (typedText.length() > 0) {
    typedText.remove(typedText.length() - 1);
  }
}

void toggleCapsLock() {
  capsLock = !capsLock;
}

void addSpace() {
  typedText += " ";
}

void addNewline() {
  typedText += "\n";
}
