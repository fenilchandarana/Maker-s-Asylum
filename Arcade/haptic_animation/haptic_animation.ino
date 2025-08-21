#include <U8glib.h>

#define BUTTON1_PIN 2   // Increase intensity
#define BUTTON2_PIN 4   // Decrease intensity
#define MOTOR_PIN   5   // Haptic motor (PWM)

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

int level = 0;          // 0 to 4 signal bars

void setup() {
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
  if (digitalRead(BUTTON1_PIN) == HIGH) {
    level++;
    if (level > 4) level = 4;
  }

  if (digitalRead(BUTTON2_PIN) == HIGH) {
    level--;
    if (level < 0) level = 0;
  }

  // Map level to exact PWM values (no leftover small PWM noise)
  int pwmValue = 0;
  switch (level) {
    case 0: pwmValue = 0; break;       // Off
    case 1: pwmValue = 64; break;      // ~25%
    case 2: pwmValue = 128; break;     // ~50%
    case 3: pwmValue = 192; break;     // ~75%
    case 4: pwmValue = 255; break;     // 100%
  }
  analogWrite(MOTOR_PIN, pwmValue);

  drawSignalBars(level);
  delay(150);   // debounce + refresh
}

void drawSignalBars(int levels) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(20, 10, "Signal Strength");

    // draw 4 bars like cellular network
    for (int i = 0; i < 4; i++) {
      int barHeight = (i + 1) * 8;
      int x = 40 + i * 12;
      int y = 60 - barHeight;
      if (i < levels) {
        u8g.drawBox(x, y, 8, barHeight);  // filled bar
      } else {
        u8g.drawFrame(x, y, 8, barHeight); // empty bar outline
      }
    }
  } while (u8g.nextPage());
}
