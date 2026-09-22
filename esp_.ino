#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Haseeb";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com"; // Free public broker

WiFiClient espClient;
PubSubClient client(espClient);

const int LIGHT_PIN = 25;
const int FAN_PIN = 26;

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Check which topic the message came in on
  if (String(topic) == "myhome/esp32/light") {
    if (message == "on") digitalWrite(LIGHT_PIN, HIGH);
    else digitalWrite(LIGHT_PIN, LOW);
  }
  else if (String(topic) == "myhome/esp32/fan") {
    if (message == "on") digitalWrite(FAN_PIN, HIGH);
    else digitalWrite(FAN_PIN, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client_Unique_ID_12345")) {
      client.subscribe("myhome/esp32/light");
      client.subscribe("myhome/esp32/fan");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}