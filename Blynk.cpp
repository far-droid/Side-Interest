#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Define WiFi credentials
const char *ssid = "your_ssid";
const char *password = "your_password";

// Define MQTT broker details
const char *mqtt_server = "your_mqtt_server_ip";
const int mqtt_port = 1883;
const char *mqtt_username = "your_mqtt_username";
const char *mqtt_password = "your_mqtt_password";

// Define Blynk authentication token
char auth[] = "your_blynk_auth_token";

// Define LDR, raindrop, and limit switch pins
const int LDR_PIN = A0;
const int RAIN_PIN = 2;
const int LIMIT_SWITCH_PIN = 3;

// Define relay and LED pins
const int RELAY_PIN = 4;
const int LED_PIN = 5;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  // initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to the Blynk server
  Blynk.begin(auth, ssid, password);

  // Set up the MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback([](char *topic, byte *payload, unsigned int length) {});

  // set LDR, raindrop, and limit switch pins as input pins
  pinMode(LDR_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);

  // set relay and LED pins as output pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    // Connect to the MQTT broker
    if (client.connect("ArduinoClient", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Connection to MQTT broker failed");
      delay(1000);
      return;
    }
  }

  // Read values from LDR, raindrop, and limit switch sensors
  int ldrValue = analogRead(LDR_PIN);
  int rainValue = digitalRead(RAIN_PIN);
  int limitSwitchValue = digitalRead(LIMIT_SWITCH_PIN);

  // Map the LDR value to a percentage value
  int ldrPercent = map(ldrValue, 0, 1023, 0, 100);

  // Publish the LDR value

client.publish("/ldr", String(ldrPercent).c_str());
Serial.print("LDR value: ");
Serial.println(ldrPercent);

// Publish the raindrop value
if (rainValue == LOW) {
client.publish("/rain", "1");
Serial.println("Rain detected");
} else {
client.publish("/rain", "0");
Serial.println("No rain detected");
}

// Publish the limit switch value
if (limitSwitchValue == LOW) {
client.publish("/limit_switch", "1");
Serial.println("Limit switch activated");
} else {
client.publish("/limit_switch", "0");
Serial.println("Limit switch deactivated");
}

// Read virtual pin values from the Blynk app
int relayValue = Blynk.getValue(V0);
int ledValue = Blynk.getValue(V1);

// Set the state of the relay and LED based on the values read from the Blynk app
digitalWrite(RELAY_PIN, relayValue.asInt());
digitalWrite(LED_PIN, ledValue.asInt());

// Keep the connection to the MQTT broker alive
client.loop();

// Wait for 100 milliseconds
delay(100);
}
