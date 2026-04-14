#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <Arduino_MKRENV.h>
#include "arduino_secrets.h"

/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;

void connectWiFi();
void connectMQTT();
void publishMessage();
void onMessageReceived(int messageSize);
void reportLedState(const char* state);
void updateLedShadow(const char* state);
unsigned long getTime();

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial);

  if (!ENV.begin()) {
    Serial.println("ENV Shield not detected!");
    while (true) { }
  }

  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  //
  // mqttClient.setId("clientId");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  // publish a message roughly every 5 seconds.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();

    publishMessage();
  }
}

unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("$aws/things/Gabriel_MKRWiFi1010/shadow/update/delta");
}

void publishMessage() {
  float temperature = ENV.readTemperature();
  Serial.println("Publishing message");

  // send message, the Print interface can be used to set the message contents
  StaticJsonDocument<256> doc;
  doc["ts"]         = millis();
  doc["temperature"] = temperature;
  //doc["humidity"]    = 48.2;
  //doc["press"]       = 100;

  // Serialize to string
  char payload[256];
  size_t n = serializeJson(doc, payload, sizeof(payload));
  if (n == 0) {
    Serial.println("JSON serialization failed (buffer too small?)");
    delay(2000);
    return;
  }
  // Publish
  Serial.print("Publishing to "); Serial.print("arduino/outgoing"); Serial.print(": ");
  Serial.println(payload);
  mqttClient.beginMessage("arduino/outgoing");
  mqttClient.write((const uint8_t*)payload, n);
  mqttClient.endMessage();
  
  // Update shadow desired LED state
  if (temperature > 28.0) {
    updateLedShadow("ON");
  } else {
    updateLedShadow("OFF");
  }


}

void onMessageReceived(int messageSize) {
  String payload = mqttClient.readString();
  Serial.println(payload);

  if (payload.indexOf("\"led\":\"ON\"") > 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    reportLedState("ON");
  } else if (payload.indexOf("\"led\":\"OFF\"") > 0) {
    digitalWrite(LED_BUILTIN, LOW);
    reportLedState("OFF");
  }
}

void reportLedState(const char* state) {
  mqttClient.beginMessage("$aws/things/Gabriel_MKRWiFi1010/shadow/update");
  mqttClient.print("{\"state\":{\"reported\":{\"led\":\"");
  mqttClient.print(state);
  mqttClient.print("\"}}}");
  mqttClient.endMessage();
}

void updateLedShadow(const char* state) {
  mqttClient.beginMessage("$aws/things/Gabriel_MKRWiFi1010/shadow/update");
  mqttClient.print("{\"state\":{\"desired\":{\"led\":\"");
  mqttClient.print(state);
  mqttClient.print("\"}}}");
  mqttClient.endMessage();
}