#include <Arduino.h>
#include <WiFi.h>
//#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>


//const char ssid = "ESP32-Access-Point";
//const char password = "123456789";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


// the number of the LED pin
const int ledPin = 16;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

void turnLightONOFF(){
  if (Serial.available() > 0){
    //send data only when you receive data:
    Serial.write(0x01);
    Serial1.write(0x01);
  }
}

void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
  Serial1.begin(115200, SERIAL_8N1, 9, 10);
  Serial.begin(115200);
}
 
void loop(){
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }
  Serial.write(0x02);

  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);   
    delay(15);
  }
  turnLightONOFF();
  Serial.write(0x03);
}