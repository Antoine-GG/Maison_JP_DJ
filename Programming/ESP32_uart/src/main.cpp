#include <Arduino.h>
#include <stdio.h>
#include <HardwareSerial.h>


HardwareSerial SerialPort(2);

void setup() {
    //configure usart for port d12 as RX and d13 as TX
    SerialPort.begin(9600, SERIAL_8N1, 16, 17);
    Serial.begin(115200);  // Adjust the baud rate as needed
}

void loop() {
    void loop() {
    // Read data from UART1 (D3 for RX)
    if (Serial1.available()) {
        String receivedData = SerialPort.readStringUntil('\n');
        if (receivedData.length() > 0) {
            Serial.println("Received from UART1 (D3): " + receivedData);
        }
    }

}
    //write a for loop to flaash a LED on pin d34
    for (int i = 0; i < 10; i++) {
        digitalWrite(34, HIGH);
        delay(100);
        digitalWrite(34, LOW);
        delay(100);
    }
}