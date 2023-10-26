#include <Arduino.h>
#include <stdio.h>

void setup() {
    Serial.begin(9600);  // Adjust the baud rate as needed
}

void loop() {
    if (Serial.available()) {
        char receivedChar = Serial.read();
        // Process received data here
    }
}