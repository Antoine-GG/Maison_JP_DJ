#include <Arduino.h>
#include <stdio.h>

void setup() {
    //configure usart for port d12 as RX and d13 as TX
    Serial.begin(9600, SERIAL_8N1, 12, 13);
    //Serial.begin(9600,);  // Adjust the baud rate as needed
}

void loop() {
    if (Serial.available()) {
        char receivedChar = Serial.read();
        // Process received data here
    }
    //write a for loop to flaash a LED on pin d34
    for (int i = 0; i < 10; i++) {
        digitalWrite(34, HIGH);
        delay(100);
        digitalWrite(34, LOW);
        delay(100);
    }
}