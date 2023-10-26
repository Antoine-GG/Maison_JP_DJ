#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

// put function declarations here:
int myFunction(int, int);

void initUART(void) {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2, 1024, 0, 0, NULL, 0);
    uart_set_baudrate(UART_NUM_2, 19200);
    uart_set_rx_timeout(UART_NUM_2, 1000);
}

void readUART(void) {
    uint8_t data[1024];
    int len = uart_read_bytes(UART_NUM_2, data, sizeof(data), 100 / portTICK_RATE_MS);
    if (len > 0) {
        printf("Received %d bytes: ", len);
        for (int i = 0; i < len; i++) {
            printf("%c", data[i]);
        }
        printf("\n");
    }
}

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  //configure UART for TX2 RX2
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
 //initUART();

}

void loop() {
  //readUART();
  // put your main code here, to run repeatedly:
  //read from UART
  if (Serial2.available()) {
   Serial2.write(Serial2.read());
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}