enum class Mode : uint8_t { Slow, Medium, Fast };

Mode mode = Mode::Medium;

void applyMode(Mode m) {
  switch (m) {
    case Mode::Slow:
      Serial.println("Mode: Slow (800 ms)");
      break;
    case Mode::Medium:
      Serial.println("Mode: Medium (400 ms)");
      break;
    case Mode::Fast:
      Serial.println("Mode: Fast (120 ms)");
      break;
    default:
      // normally unreachable if all cases covered
      break;
  }
}
