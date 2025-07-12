const int piezoPins[5] = {1, 9, 10, 6, 12};
const char piezoLetters[5] = {'a', 'b', 'c', 'd', 'e'};

// Store previous states
int previousState[5] = {0, 0, 0, 0, 0};

// Debounce lockout timers
unsigned long lastTriggerTime[5] = {0, 0, 0, 0, 0};
const unsigned long debounceDelay = 100; // milliseconds

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
      if (currentTime - lastTriggerTime[i] > debounceDelay) {
        Serial.println(piezoLetters[i]);
        lastTriggerTime[i] = currentTime; // Reset debounce timer
      }
    }

    previousState[i] = currentState;
  }
  delay(1);
}
