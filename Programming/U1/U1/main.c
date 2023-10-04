/*
 * main.c
 *
 * Created: 9/20/2023 7:20:41 AM
 *  Author: JP Toutant
 */ 
#define F_CPU 1000000UL

#include <xc.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <stdio.h>								/* Include standard I/O header file */
#include <string.h>
#include "I2C_Slave_H_File.h"

#define SLAVE_ADDRESS 0x3C//addresse i2c de U2

/*
 * main.c
 *
 * Created: 10/4/2023 1:49:19 PM
 *  Author: JP Toutant
 */ 

#include <xc.h>
#include <util/twi.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

uint8_t window1;
uint8_t window2;

//ports + window states
void iniIOports(void){
	// Set PD7 and 6 as inputs
	DDRD &= ~(1 << PD6) | (1 << PD7);
	// Enable the internal pull-up resistor for PD7
	PORTD |= (1 << PD6) | (1 << PD7);
	// Set PD6 (Pin 6 of Port D) as an output (for an LED)
	DDRB |= (1 << PB1) | (1 << PB2);
}
int setWindow(uint8_t inPin, uint8_t ledPin){
	// Check if PD7 is clear (low)
	if (!(PIND & (1 << inPin)))
	{
		// If PD7 is clear, set PD6 (turn on the LED)
		PORTB &= ~(1 << ledPin);
		return 0;
		} else {
		// If PD7 is set, clear PD6 (turn off the LED)
		PORTB |= (1 << ledPin);
		return 1;
	}
}

int main(void)
{
	iniIOports();
    
	while(1)
    {
		window1 = setWindow(PD7, PB2);
		window2 = setWindow(PD6, PB1);
		
		// Add a delay for debouncing or to prevent rapid toggling
		_delay_ms(100);
        //TODO:: Please write your application code 
    }
}

void initIOports(){
	
	// Set PD7 and PD6 as input pin
	DDRD &= ~((1 << PD7)|(1 << PD6));
	
	//Set PB1 and PB2 as output pins
	DDRB |= ((1 << PB1)|(1 << PB2));
	
	// Enable pull-up resistor for input pins
	PORTD |= ((1 << PD7)|(1 << PD6));

}

int main(void) {
	initIOports();
	int drapeau=0;
	int8_t instructionCode = 0;
	uint8_t window1 = 0;
	uint8_t window2 = 0;
	uint8_t windows = 0;

	I2C_Slave_Init(SLAVE_ADDRESS);
	initIOports();
	
	while (1) {
		//pick door status on pin PD7
		window2 = PIND & (1 << PD7);
		//put door status out on PB1
		window1 = PIND & (1 << PD6);


		if(window1 == 1){
			PORTB &= ~(1 << PB1); //debug led for tension divider tweaks
		}
		else{
			PORTB |= (1 << PB1); 
		}
		if(window2 == 1){
			PORTB &= ~(1 << PB2); //debug led for tension divider tweaks
		}
		else{
			PORTB |= (1 << PB2); 
		}
		windows = (window1 << 1) | window2;
		switch(I2C_Slave_Listen())				/* Check for any SLA+W or SLA+R */
		{
			case 0://receive
			{
				do
				{
	
					instructionCode = I2C_Slave_Receive();/* Receive data byte*/
					if(instructionCode==0xAE) drapeau=1;  // vérifier si c'est 0xAE (code) alors autoriser la transmission
				} while (instructionCode != -1);			/* Receive until STOP/REPEATED START received */
				
				break;
			}
			
			case 1://transmit
			{
				int8_t Ack_status;
		        if(drapeau==1){ //si bon code envoi l'état de PB0
				do
					{   
						Ack_status = I2C_Slave_Transmit(windows);	/* Send data byte */
				
					} while (Ack_status == 0);		/* Send until Acknowledgment is received */
				
					drapeau=0;//reset pour lire une autre donnée
				}
				break;
			}
			default:
				break;
		}

	}

	return 0;
}
