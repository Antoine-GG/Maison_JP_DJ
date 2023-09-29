/*
 * main.c
 *
 * Created: 9/20/2023 7:20:41 AM
 *  Author: JP Toutant
 */ 
#define F_CPU 1000000UL

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>								/* Include standard I/O header file */
#include <string.h>
#include "I2C_Slave_H_File.h"

#define SLAVE_ADDRESS 0x3C//addresse i2c de U2

void initIOports(){
	// Set PD7 and PD6 as input pin
	DDRD &= ~((1 << PD7)|(1 << PD6));
	//Set PB1 and PB2 as output pins
	DDRB |= ((1 << PB1)|(1 << PB2));
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

		if(window1 == 0){
			PORTB &= ~(1 << PB1); //debug led for tension divider tweaks
		}
		else{
			PORTB |= (1 << PB1); 
		}
		if(window2 == 0){
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
