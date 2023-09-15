/*
 * main.c
 *
 * Created: 9/14/2023 3:46:22 PM
 *  Author: JP Toutant, ChatGPT
 */ 

#include <xc.h>
#include <util/delay.h>
#define F_CPU 1000000UL


#define LATCH_PIN PB0
#define CLOCK_PIN PD2
#define DATA_PIN PD5

void initIOports(){
	
		// Initialize I/O pins
		DDRB |= (1 << LATCH_PIN); // Latch pin as output
		DDRD |= (1 << CLOCK_PIN); // Clock pin as output
		DDRD &= ~(1 << DATA_PIN); // Data pin as input
		
		// Enable pull-up resistor for data pin
		PORTD |= (1 << DATA_PIN);
}

// Function to read data from the shift registers
uint16_t readShiftRegisters() {
	uint16_t data = 0;
	
	// Set latch pin low to start data transfer
	PORTB &= ~(1 << LATCH_PIN);
	
	// Shift in data from CD4021 shift registers
	for (int i = 0; i < 16; i++) {
		// Pulse the clock pin to shift in the next bit
		PORTD |= (1 << CLOCK_PIN);
		_delay_us(100); // Adjust delay as needed based on your clock frequency
		PORTD &= ~(1 << CLOCK_PIN);
		
		// Read the data pin and store the bit
		data |= ((PIND & (1 << DATA_PIN)) >> DATA_PIN) << i;
	}
	
	// Set latch pin high to stop data transfer
	PORTB |= (1 << LATCH_PIN);
	
	return data;
}

int main() {

	while (1) {
		// Read data from shift registers
		uint16_t shiftRegisterData = readShiftRegisters();
		
		// Process and use the received data as needed
		// You can print it, send it to other devices, or perform any desired operations
		
		// Delay between data reads (adjust as needed)
		_delay_ms(1000); // Example: 1-second delay
	}
	
	return 0;
}