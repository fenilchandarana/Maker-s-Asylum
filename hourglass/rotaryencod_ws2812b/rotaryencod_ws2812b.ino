#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define inputCLK 25
#define inputDT 26
#define inputButton 27

#define LED_PIN 13
#define NUM_LEDS 42

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int currentStateCLK;
int previousStateCLK;

int ledIndex = 0;         // Current LED position
bool selecting = false;   // False = preview mode, True = selection mode
int startIndex = -1;      // Starting index of selection

int buttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(inputCLK, INPUT);
  pinMode(inputDT, INPUT);
  pinMode(inputButton, INPUT_PULLUP);

  previousStateCLK = digitalRead(inputCLK);

  strip.begin();
  strip.setBrightness(60);
  strip.show(); // All OFF

  Serial.begin(9600);
}

void loop() {
  // ----- Handle Button Press -----
  int reading = digitalRead(inputButton);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (!selecting) {
          // First press
          startIndex = ledIndex;
          selecting = true;
          Serial.print("Start selection at LED ");
          Serial.println(startIndex + 1);  // Adjusted for 1-based count
        } else {
          // Second press
          selecting = false;
          Serial.print("End selection at LED ");
          Serial.println(ledIndex + 1);  // Adjusted for 1-based count

          int from = min(startIndex, ledIndex);
          int to = max(startIndex, ledIndex);

          for (int i = from; i <= to; i++) {
            strip.setPixelColor(i, strip.Color(255, 0, 0)); // Red
          }
          strip.show();
        }
      }
    }
  }
  lastButtonState = reading;

  // ----- Handle Rotary Encoder -----
  currentStateCLK = digitalRead(inputCLK);

  if (currentStateCLK != previousStateCLK) {
    if (digitalRead(inputDT) == currentStateCLK) {
      // CCW
      if (ledIndex > 0) ledIndex--;
    } else {
      // CW
      if (ledIndex < NUM_LEDS - 1) ledIndex++;
    }

    Serial.print(selecting ? "Selecting: " : "Preview: ");
    Serial.println(ledIndex + 1);  // Adjusted for 1-based count

    if (!selecting) {
      // Preview mode: only one LED ON
      strip.clear();
      strip.setPixelColor(ledIndex, strip.Color(0, 0, 255)); // Blue
      strip.show();
    } else {
      // Selection mode: keep glowing as we pass
      strip.setPixelColor(ledIndex, strip.Color(255, 0, 0)); // Red
      strip.show();
    }
  }

  previousStateCLK = currentStateCLK;
}
