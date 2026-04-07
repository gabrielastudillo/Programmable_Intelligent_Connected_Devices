#include <Arduino_MKRENV.h>

const int N = 10;
float temps[N];
int indexPos = 0;

const float ALERT_TEMP = 26.0;  // check your room temp

void setup() {
  Serial.begin(9600);

  if (!ENV.begin()) {
    Serial.println("ENV Shield not detected!");
    while (true) { }
  }

  Serial.println("Starting...");
}

void loop() {
  float t = ENV.readTemperature();  // collected data
  temps[indexPos] = t;
  indexPos++;

  // REAL-TIME REPORT (only when needed)
  if (t >= ALERT_TEMP) {
    Serial.print("!!! REAL-TIME ALERT temp=");
    Serial.println(t);
  }

  delay(1000);

  // SUMMARY REPORT every N samples
  if (indexPos == N) {
    reportToSerial(average(temps, N),minimum(temps, N),maximum(temps, N));
    indexPos = 0;
  }
}

float average(float arr[], int size) {
  float sum = 0;
  for (int i = 0; i < size; i++) sum += arr[i];
  return sum / size;
}
float minimum(float arr[], int size) {
  float mn = arr[0];
  for (int i = 1; i < size; i++) if (arr[i] < mn) mn = arr[i];
  return mn;
}
float maximum(float arr[], int size) {
  float mx = arr[0];
  for (int i = 1; i < size; i++) if (arr[i] > mx) mx = arr[i];
  return mx;
}

void reportToSerial(float avg, float mn, float mx) {
  Serial.print("avg="); Serial.print(avg);
  Serial.print(" min="); Serial.print(mn);
  Serial.print(" max="); Serial.println(mx);
}