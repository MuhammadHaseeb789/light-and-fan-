#include <WiFi.h>
#include <PubSubClient.h>

// --- WiFi Credentials ---
const char* ssid = "Haseeb";
const char* password = "12345678";

// --- MQTT Broker Settings ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// --- GPIO Pin Definitions for Relays ---
const int LIGHT_PIN = 25; // Connect Relay 1 (Light) here
const int FAN_PIN = 26;   // Connect Relay 2 (Fan) here

// --- MQTT Topics ---
const char* TOPIC_LIGHT_SET = "myhome/esp32/light/set";
const char* TOPIC_LIGHT_STATUS = "myhome/esp32/light/status";

const char* TOPIC_FAN_SET = "myhome/esp32/fan/set";
const char* TOPIC_FAN_STATUS = "myhome/esp32/fan/status";

WiFiClient espClient;
PubSubClient client(espClient);

// Function declarations
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200);

  // Initialize Relay Pins
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  
  // Set default state to OFF (LOW). Change to HIGH if your relay module is Active-Low.
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  // Connect to Wi-Fi and MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// --- Connect to Wi-Fi ---
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// --- MQTT Callback: Triggered when a message is received ---
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  // Handle Light Command
  if (String(topic) == TOPIC_LIGHT_SET) {
    if (message == "on") {
      digitalWrite(LIGHT_PIN, HIGH);
      client.publish(TOPIC_LIGHT_STATUS, "on", true); // Retain status
    } else {
      digitalWrite(LIGHT_PIN, LOW);
      client.publish(TOPIC_LIGHT_STATUS, "off", true);
    }
  }
  
  // Handle Fan Command
  else if (String(topic) == TOPIC_FAN_SET) {
    if (message == "on") {
      digitalWrite(FAN_PIN, HIGH);
      client.publish(TOPIC_FAN_STATUS, "on", true); // Retain status
    } else {
      digitalWrite(FAN_PIN, LOW);
      client.publish(TOPIC_FAN_STATUS, "off", true);
    }
  }
}

// --- Reconnect to MQTT Broker if disconnected ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a unique client ID using chip ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      // Subscribe to command topics
      client.subscribe(TOPIC_LIGHT_SET);
      client.subscribe(TOPIC_FAN_SET);
      
      // Publish current physical states on startup/reconnect
      client.publish(TOPIC_LIGHT_STATUS, digitalRead(LIGHT_PIN) == HIGH ? "on" : "off", true);
      client.publish(TOPIC_FAN_STATUS, digitalRead(FAN_PIN) == HIGH ? "on" : "off", true);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
