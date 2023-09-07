/*
 * main.c
 *	
 *  Created: 1er Septembre 2023
 *  Author: JP Toutant
 * 	Modified by: Jérémy JBDD
 */ 


#include <xc.h>
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned int door1 = 0;
unsigned int door2 = 0;

// Function prototypes
void initInterrupts();

int main(void) {
	
	// Configure PC5 as a debug output
	DDRC |= (1 << PC5);
	
	// Initialize external interrupts
	initInterrupts();

	// Enable global interrupts
	sei();

	// Main loop
	while (1) {
		
		if(door1 == 1){
			// Turn on the LED (set PD2 high)
			PORTC |= (1 << PC5);
		}
		else{
			// Turn off the LED (clear PD2)
			PORTC &= ~(1 << PC5);			
		}

		if (door2 == 1) {
            // Turn on the LED (set PC5 high)
            PORTC |= (1 << PC5);
        } else {
            // Turn off the LED (clear PC5)
            PORTC &= ~(1 << PC5);
        }
	}

	return 0;
}

void initInterrupts() {
	// Configure PD2 (INT0) and PD3 (INT1) as inputs
	DDRD &= ~((1 << PD2) | (1 << PD3));
	//input pullup
	 PORTD |= (1 << PD2);  // Enable pull-up resistor (set the

	// Enable external interrupts for INT0 and INT1
	EIMSK |= (1 << INT0) | (1 << INT1);

	// Interrupt configs
	
	// For example, to trigger on falling edges:
	//EICRA |= (1 << ISC01) | (1 << ISC11);
	
	// Configure interrupt triggering logic for status changes (both edges)
	//EICRA |= (1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11);
	
	//setup for low level trigger
	EICRA &= ~((1 << ISC21) | (1 << ISC20) | (1 << ISC31) | (1 << ISC30));

	// Clear any pending interrupts
	EIFR = (1 << INTF0) | (1 << INTF1);
}

// Interrupt service routines
ISR(INT0_vect) {
	// This code executes when INT0 (PD2) is triggered
	// Handle the door detector on PD2 here
	
	//flip the boolean value of door1
	if(door1 ==1){
		door1 = 0;
	}
	else
	{
		door1 = 1;
	}
	
}

ISR(INT1_vect) {
	// This code executes when INT1 (PD3) is triggered
	// Handle the door detector on PD3 here
	
	//flip the boolean value of door1
	if(door1 ==1){
		door2 = 0;
	}
	else
	{
		door2 = 1;
	}
}