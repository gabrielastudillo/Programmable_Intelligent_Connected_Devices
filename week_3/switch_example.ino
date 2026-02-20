void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Commands: (o)N, (f)OFF, (t)OGGLE");
}

bool led = false;

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case 'o': // on
      case 'O':
        led = true;
        Serial.println("LED ON");
        break;

      case 'f': // off
      case 'F':
        led = false;
        Serial.println("LED OFF");
        break;

      case 't': // toggle
      case 'T':
        led = !led;
        Serial.println("LED TOGGLED");
        break;

      default:
        // ignore newlines/carriage returns; message for anything else
        if (cmd != '\n' && cmd != '\r') {
          Serial.println("Unknown command. Use: o, f, t");
        }
        break;
    }
    digitalWrite(LED_BUILTIN, led ? HIGH : LOW);
  }
}
