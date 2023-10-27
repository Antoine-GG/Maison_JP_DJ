// Date: 2021-09-30

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// the number of the LED pin
const int ledPin = 34;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;


//const char ssid = "ESP32-Access-Point";
//const char password = "123456789";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
  // configure Serial for pins RX0 and TX0
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 12, 13);
}

void turnLightONOFF(){
  if (Serial.available() > 0){
    //send data only when you receive data:
    Serial.write(0x01);
    Serial1.write(0x01);
  }
}
 
void loop(){

  if (Serial1.available() > 0) {
    // Si des données sont disponibles sur la liaison UART depuis l'Atmega328P
    char receivedByte = Serial2.read(); // Lire la donnée reçue

    // Afficher la donnée reçue dans le moniteur série Arduino IDE
    Serial.print(" Reçu : ");
    Serial.println(receivedByte);

  }
}
