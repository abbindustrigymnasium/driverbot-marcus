#define motorPinLeftSpeed 4  // D? !0
#define motorPinLeftDir 0
 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <MQTTClient.h>
 
 
#define WIFI_SSID "ABBgym_2.4"
#define WIFI_PASSWORD "mittwifiarsabra"

const char WIFI_SSID[] = "ABBgym_2.4";     // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "mittwifiarsabra";  // CHANGE TO YOUR WIFI PASSWORD
 
// MQTT Broker
const char *mqtt_broker = "https://maqiatto.com/";
const char *topic = "marcus.persson@hitachigymnasiet.se/driverbot";
const char *mqtt_username = "marcus.persson@hitachigymnasiet.se";
const char *mqtt_password = "GreekMythology";
const int mqtt_port = 1883;
 
const char PUBLISH_TOPIC[] = "marcus.persson@hitachigymnasiet.se/driverbot";    // CHANGE IT AS YOU DESIRE
const char SUBSCRIBE_TOPIC[] = "YOUR-NAME-esp8266-001/loopback";  // CHANGE IT AS YOU DESIRE

const int PUBLISH_INTERVAL = 5000;

void setup() {
  Serial.begin(9600);
  //delay so the serial monitor can keep up 🙏
  delay(500);
  // connecting to a WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connection established to: " );
  Serial.print(WIFI_SSID);

  connectToMQTT();
 
  // put your setup code here, to run once:
  pinMode(motorPinLeftSpeed, OUTPUT);
  pinMode(motorPinLeftDir, OUTPUT);
  analogWrite(motorPinLeftSpeed, 1024);
  digitalWrite(motorPinLeftDir, HIGH);
 
}
 
void loop() {
  digitalWrite(motorPinLeftDir, HIGH);
 
  mqtt.loop();

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToMQTT();
    lastPublishTime = millis();
  }
 
}

void connectToMQTT() {
  // Connect to the MQTT broker
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);

  // Create a handler for incoming messages
  mqtt.onMessage(messageHandler);

  Serial.print("ESP8266 - Connecting to MQTT broker");

  while (!mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if (!mqtt.connected()) {
    Serial.println("ESP8266 - MQTT broker Timeout!");
    return;
  }

  // Subscribe to a topic, the incoming messages are processed by messageHandler() function
  if (mqtt.subscribe(SUBSCRIBE_TOPIC))
    Serial.print("ESP8266 - Subscribed to the topic: ");
  else
    Serial.print("ESP8266 - Failed to subscribe to the topic: ");

  Serial.println(SUBSCRIBE_TOPIC);
  Serial.println("ESP8266 - MQTT broker Connected!");
}

void sendToMQTT() {
  StaticJsonDocument<200> message;
  message["timestamp"] = millis();
  message["data"] = analogRead(0);  // Or you can read data from other sensors
  char messageBuffer[512];
  serializeJson(message, messageBuffer);

  mqtt.publish(PUBLISH_TOPIC, messageBuffer);

  Serial.println("ESP8266 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("ESP8266 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);
}