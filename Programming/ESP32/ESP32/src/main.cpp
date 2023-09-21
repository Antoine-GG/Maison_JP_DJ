#include <Arduino.h>
#include <WiFi.h>
//#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <HardwareSerial.h> //uart starts here

struct DeviceStatus {
  bool window1;
  bool window2;
  bool door;
  bool lock;
  bool light;
};

HardwareSerial Serial1(1);

//const char ssid = "ESP32-Access-Point";
//const char password = "123456789";
DeviceStatus status;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

parseReceivedMessage(String message) {
  //check message integrity
  if(message.length() != 8) return;
  if(message.charAt(0) != '&') return;
  if(message.charAt(4) != ':') return;
  if(message.charAt(7) != '$'){

  }

  // Extract the status of window1, door, lock, and light
  if (message.startsWith("<W") && message.length() >= 9) {
    if(message.charAt(1) == '1'){
      status.window1 = true;
    }
    if(message.charAt(2) == '1'){
      status.window2 = true;
    }
    if(message.charAt(3) == '1'){
      status.door = true;
    }
    if(message.charAt(5) == '1'){
      status.lock = true;
    }
    if(message.charAt(6) == '1'){
      status.light = true;
    }
    // Process the status of the devices
    // You can use window1Status, doorStatus, lockStatus, and lightStatus
  }
}
void setup(){
  Serial1.begin(9600, SERIAL_8N1, 1, 3);
}

void loop()
{
  // Check if data is available to read
  if (Serial1.available())
  {
    String data = Serial1.readString();
    // Do something with 'data'
  }
  
  // Write data
  String data_to_send = "Hello from ESP32!";
  Serial1.println(data_to_send);
}