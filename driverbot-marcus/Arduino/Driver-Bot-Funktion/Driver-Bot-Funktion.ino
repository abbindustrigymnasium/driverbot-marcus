#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

Servo S;  // Create a servo object

// Update these with values suitable for your network and Maqiatto account
const char* ssid = "ABBgym_2.4";
const char* password = "mittwifiarsabra";
const char* mqtt_server = "maqiatto.com";
const char* mqtt_user = "marcus.persson@hitachigymnasiet.se";
const char* mqtt_password = "GreekMythology";
const char* subscribe_topic = "marcus.persson@hitachigymnasiet.se/driverbot";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Convert payload to string and trim
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim(); // Remove any leading/trailing whitespace
  Serial.println(message);

  // Check if the message is "w" and run motor
  if (message.equals("s")) {
    digitalWrite(D3, LOW); // set direction forward
    analogWrite(D1, 90);
  } else if (message.equals("w")) {
    // Spin the motor backward
    digitalWrite(D3, HIGH); // set direction backward
    analogWrite(D1, 90);
  } else if (message.equals("q")) {
    // Stop the motor
    analogWrite(D1, 0);
  } else if (message.equals("a")) {
    // Move the servo
    S.write(180);      // Move servo to 0 degrees
  } else if (message.equals("d")) {
    S.write(0);    // Move servo to 180 degrees
  } else if (message.equals("e")) {
    S.write(90);    // Move servo to 90 degrees
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     
      // ... and resubscribe
      client.subscribe(subscribe_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D1, OUTPUT);   // Initialize D1 pin as an output for motor control
  pinMode(D3, OUTPUT);   // Initialize D3 pin as an output for motor direction
  S.attach(D6);          // Attach the servo to D2
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
