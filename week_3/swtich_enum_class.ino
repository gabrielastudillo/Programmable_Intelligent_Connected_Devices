enum class Mode : uint8_t { Off, On, Blink };

Mode mode = Mode::Off;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  switch (mode) {
    case Mode::Off:
      digitalWrite(LED_BUILTIN, LOW);
      break;

    case Mode::On:
      digitalWrite(LED_BUILTIN, HIGH);
      break;

    case Mode::Blink: {
      static unsigned long last = 0;
      static bool led = false;
      unsigned long now = millis();
      if (now - last >= 300) {
        led = !led;
        digitalWrite(LED_BUILTIN, led ? HIGH : LOW);
        last = now;
      }
      break;
    }
  }

  // Example: change mode from Serial
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'o') mode = Mode::On;
    else if (c == 'f') mode = Mode::Off;
    else if (c == 'b') mode = Mode::Blink;
  }
}
