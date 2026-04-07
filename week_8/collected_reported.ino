#include <Arduino_MKRENV.h>

const int N = 10;           // how many samples to store
float temps[N];             // array to store temperatures
int indexPos = 0;           // next position to write into the array

void setup() {
  Serial.begin(9600);

  // Start the ENV shield sensors
  if (!ENV.begin()) {
    Serial.println("ENV Shield not detected!");
    while (true) { }        // stop here
  }

  Serial.println("Collecting data...");
}

void loop() {
  // 1) COLLECT data (one sample)
  float t = ENV.readTemperature();   // Celsius
  temps[indexPos] = t;
  indexPos++;

  Serial.print("Collected temp: ");
  Serial.println(t);

  delay(1000);  // 1 second (simple timing for beginners)

  // 2) When we collected N samples, REPORT a summary
  if (indexPos == N) {
    float avg = average(temps, N);
    float mn  = minimum(temps, N);
    float mx  = maximum(temps, N);

    Serial.println("---- REPORTED SUMMARY (last 10 seconds) ----");
    Serial.print("Average: "); Serial.println(avg);
    Serial.print("Min: ");     Serial.println(mn);
    Serial.print("Max: ");     Serial.println(mx);
    Serial.println("-------------------------------------------");

    indexPos = 0; // start collecting again (overwrite old data)
  }
}

// Compute average
float average(float arr[], int size) {
  float sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum / size;
}

// Compute minimum
float minimum(float arr[], int size) {
  float mn = arr[0];
  for (int i = 1; i < size; i++) {
    if (arr[i] < mn) mn = arr[i];
  }
  return mn;
}

// Compute maximum
float maximum(float arr[], int size) {
  float mx = arr[0];
  for (int i = 1; i < size; i++) {
    if (arr[i] > mx) mx = arr[i];
  }
  return mx;
}