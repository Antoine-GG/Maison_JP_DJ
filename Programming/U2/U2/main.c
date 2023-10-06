/*
 * main.c
 *
 * Created: 9/20/2023 7:20:41 AM
 *  Author: JP Toutant
 */ 

#define F_CPU 1000000UL
#include <xc.h>
#include <util/twi.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <stdio.h>								
#include <string.h>
#include "I2C_Slave_H_File.h"


#define SLAVE_ADDRESS 0x5A//addresse i2c de U2

uint8_t windows;

//ports + window states
void iniIOports(void){
	// Set PD7 and 6 as inputs
	DDRD &= ~(1 << PD6) | (1 << PD7);
	// Enable the internal pull-up resistor for PD7
	PORTD |= (1 << PD6) | (1 << PD7);
	// Set PD6 (Pin 6 of Port D) as an output (for an LED)
	DDRB |= (1 << PB1) | (1 << PB2);
}
int getWindowState(uint8_t inPin, uint8_t ledPin){
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
void slave_answerPoll(uint8_t data, int8_t codeWord){
	int drapeau = 0;
	switch(I2C_Slave_Listen())				/* Check for any SLA+W or SLA+R */
		{
			case 0://receive
			{
				do
				{
	
					codeWord = I2C_Slave_Receive();/* Receive data byte*/
					if(codeWord==0x25) drapeau=1;  // vérifier si c'est 0x25 (code) alors autoriser la transmission
				} while (codeWord != -1);			/* Receive until STOP/REPEATED START received */
				
				break;
			}
			
			case 1://transmit
			{
				int8_t Ack_status;
		        if(drapeau==1){ //si bon code envoi l'état de PB0
				do
					{  Ack_status = I2C_Slave_Transmit(data);	/* Send data byte */
				
					} while (Ack_status == 0);		/* Send until Acknowledgment is received */
				
					drapeau=0;//reset pour lire une autre donnée
				}
				break;
			}
			default:
				break;
		}
}

int main(void)
{
	I2C_Slave_Init(SLAVE_ADDRESS);
	iniIOports();
    
	while(1)
    {
		// concatenation of the two window states to send LT
		windows = ((1 << getWindowState(PD7, PB2)) | getWindowState(PD6, PB1));
		// Add a delay for debouncing or to prevent rapid toggling
		_delay_ms(100);
        //TODO:: Please write your application code 
		slave_answerPoll(windows, 0x25);
    }
}