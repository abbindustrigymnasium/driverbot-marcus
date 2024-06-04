#include <ESP8266WiFi.h>
#include <Arduino.h>

#include "MQTTConnector.h"
#include "Credentials.h"

#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 5

#include <Servo.h>

Servo S;  // Create a servo object
int motorSpeed = 65; // Initial speed of the motor
unsigned long lastInputTime = 0; // Variable to store the time of the last input

void WiFiBegin(const char* ssid, const char* pass)
{
  WiFi.begin(ssid, pass);
  Serial.printf("Waiting for AP connection ...\n");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(WIFI_TIMEOUT);
    Serial.printf(".");
  }
  IPAddress ip = WiFi.localIP();
  Serial.printf("\nConnected to AP. IP : %d.%d.%d.%d\n", 
    ip[0],ip[1],ip[2],ip[3]);
}

void setup() 
{
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  WiFiBegin(STA_SSID, STA_PASS);
  MQTTBegin();

  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);
  analogWrite(D1, motorSpeed);

  S.attach(D6);  // Attach the servo to pin 6
}

void input_handler(char input) {
    lastInputTime = millis(); // Update the time of the last input
    if (input == 'W' || input == 'w') {
      // Spin the motor forward
      digitalWrite(D3, LOW); // set direction forward
      analogWrite(D1, 65);
    } 
    else if (input == 'S' || input == 's') {
      // Spin the motor backward
      digitalWrite(D3, HIGH); // set direction backward
      analogWrite(D1, 65);
    }
    else if (input == 'Q' || input == 'q') {
      // Stop the motor
      analogWrite(D1, 0);
    }
    else if (input == 'A' || input == 'a') {
      // Move the servo
      S.write(0);      // Move servo to 0 degrees
    }
    else if (input == 'D' || input == 'd') {
      S.write(180);    // Move servo to 180 degrees
    }
    else if (input == 'E' || input == 'e') {
      S.write(90);    // Move servo to 180 degrees
    }
}

void loop() 
{
  MQTTLoop();
  /*if(MQTTPublish(TOPIC, "test maqiatto!"))
  {
    Serial.printf("MQTTPublish was succeeded.\n");
  }*/
  delay(LOOP_TIMEOUT);
}
