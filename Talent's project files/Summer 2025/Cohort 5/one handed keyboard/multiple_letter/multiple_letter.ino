const int piezoPins[5] = {1, 9, 10, 6, 12};

// Letters for each piezo key
const char* piezoLetters[5] = {
  "abcd",   // piezo 1
  "efgh",   // piezo 9
  "ijklmn", // piezo 10
  "opqrst", // piezo 6
  "uvwxyz"  // piezo 12
};

// Debounce & multi-tap variables
int previousState[5] = {0, 0, 0, 0, 0};
unsigned long lastTriggerTime[5] = {0, 0, 0, 0, 0};
const unsigned long debounceDelay = 100;   // debounce for vibration
const unsigned long pauseTimeout = 500;   // pause to confirm letter

// Multi-tap tracking
int tapCount[5] = {0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) {
    pinMode(piezoPins[i], INPUT);
  }
}

void loop() {
  unsigned long currentTime = millis();

  for (int i = 0; i < 5; i++) {
    int currentState = digitalRead(piezoPins[i]);

    if (currentState == HIGH && previousState[i] == LOW) {
      // Check debounce
      if (currentTime - lastTriggerTime[i] > debounceDelay) {
        tapCount[i]++;  // Next letter
        lastTriggerTime[i] = currentTime;
      }
    }

    // If enough pause, confirm the last selected letter
    if (tapCount[i] > 0 && (currentTime - lastTriggerTime[i] > pauseTimeout)) {
      const char* letters = piezoLetters[i];
      int numLetters = strlen(letters);
      int index = (tapCount[i] - 1) % numLetters;
      Serial.println(letters[index]);
      tapCount[i] = 0; // Reset for next sequence
    }

    previousState[i] = currentState;
  }

  delay(1);
}
