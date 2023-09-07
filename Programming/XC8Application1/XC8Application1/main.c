/*
 * main.c
 *	
 *  Created: 1er Septembre 2023
 *  Author: JP Toutant et Jérémy JBDD
 */ 


#include <xc.h>
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

unsigned int window1 = 0;
unsigned int window2 = 0;

void initIOports(){

	// Set PC4 and PC5 as output pin for debug
	DDRC |= (1 << 5);
	DDRC |= (1 << 4);

	// Set PD3 and PD4 as input pins
    DDRD &= ~(1 << 3);
    DDRD &= ~(1 << 4);

    // Enable pull-up resistors on PD3 and PD4
    //PORTD |= (1 << 3);
    //PORTD |= (1 << 4);
}

// changement de stratégie nous laissons tomber les interrupts pour simplement faire un polling
int main(void){
	initIOports();
	while(1){
		//read the state of the doors at pin 3 and 4
		window1 = PIND & (1 << 3);
		window2 = PIND & (1 << 4);
		//if the door1 is open set PC5 pin to high otherwise set it to low
		if(window1 == 0){
			PORTC |= (1 << 5);
		}else{
			PORTC &= ~(1 << 5);
		}
		if(window2 == 0){
			PORTC |= (1 << 4);
		}else{
			PORTC &= ~(1 << 4);
		}
		_delay_ms(100);
		}
	return 0;
}