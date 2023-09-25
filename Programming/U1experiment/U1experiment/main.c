/*
 * main.c
 *
 * Created: 9/25/2023 12:29:49 AM
 *  Author: Utilisateur
 */ 

#include <xc.h>
#include <avr/io.h>
#include <stdio.h>								/* Include standard I/O header file */
#include <string.h>
#include "I2C_Slave_H_File.h"

#define slave 0x50

int main() {
	// Configure PB0 as an input
	DDRB &= ~(1 << PB0);

	// Enable the internal pull-up resistor for PB0
	PORTB |= (1 << PB0);

	// Create an uint8_t variable to store the button state (1 or 0)
	uint8_t buttonState = 0;
	I2C_Slave_Init(slave);
	while (1) {
		// Read the button state (active-low)
		if (!(PINB & (1 << PB0))) {
			// Button is pressed (PB0 is LOW)
			buttonState = 1;
			} else {
			// Button is not pressed (PB0 is HIGH)
			buttonState = 0;
		}
		switch(I2C_Slave_Listen()){
			case 0:
			{
				break;
			}
			case 1:
			{
				uint8_t ackstatus;
				do 
				{
					ackstatus = I2C_Slave_Transmit(buttonState);
				} while (ackstatus ==0);
					break;
			}
			default:
				break;
			}
		

		// You can now use the 'buttonState' variable for your application
		// For example, you can perform actions based on the button state.

		// Add a debounce delay here if needed to handle button bounce
	}
	return 0;
}