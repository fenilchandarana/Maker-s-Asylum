const int pins[] = {4, 15, 12, 13, 32, 33};
const int pinCount = sizeof(pins) / sizeof(pins[0]);

bool alreadyChecked[pinCount];

void setup() {
  Serial.begin(115200);
}

void loop() {
  memset(alreadyChecked, false, sizeof(alreadyChecked));

  for (int i = 0; i < pinCount; i++) {
    if (alreadyChecked[i]) continue;

    // Set this pin as OUTPUT and drive HIGH
    for (int j = 0; j < pinCount; j++) {
      if (i == j) {
        pinMode(pins[j], OUTPUT);
        digitalWrite(pins[j], HIGH);
      } else {
        pinMode(pins[j], INPUT_PULLDOWN);
      }
    }

    delay(5); // settle

    // Check which other pins read HIGH (i.e., are connected)
    bool group[pinCount] = {false};
    group[i] = true;

    for (int k = 0; k < pinCount; k++) {
      if (k != i && digitalRead(pins[k]) == HIGH) {
        group[k] = true;
      }
    }

    // If more than one pin in group, print the group
    int groupSize = 0;
    for (int g = 0; g < pinCount; g++) {
      if (group[g]) groupSize++;
    }

    if (groupSize > 1) {
      Serial.print("Detected group: ");
      for (int g = 0; g < pinCount; g++) {
        if (group[g]) {
          Serial.print(pins[g]);
          Serial.print(" ");
          alreadyChecked[g] = true;
        }
      }
      Serial.println();
    }
  }

  Serial.println("Scan complete.\n");
  delay(1000);
}
