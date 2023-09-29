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

#define SLAVE_ADDRESS 0x5A//addresse i2c de U2

void initIOports(){
	// Set PD7 as input pin
	DDRD &= ~(1 << PD6);
	//Set PB1 as output pin
	DDRB |= (1 << PB1);
}

int main(void) {
	int drapeau=0;
	uint8_t doorStatus =0;
	int8_t instructionCode = 0;

	I2C_Slave_Init(SLAVE_ADDRESS);
	initIOports();
	

	while (1) {
		//read pin PD6
		doorStatus =0;
		doorStatus = PIND & (1 << PD6);
		//put door status out on PB1
		if(doorStatus == 0){
			PORTB &= ~(1 << PB1); //debug led for tension divider tweaks
		}
		else{
			PORTB |= (1 << PB1); 
		}
		switch(I2C_Slave_Listen())				/* Check for any SLA+W or SLA+R */
		{
			case 0://receive
			{
				do
				{
	
					instructionCode = I2C_Slave_Receive();/* Receive data byte*/
					if(instructionCode==0x25)drapeau=1;  // vérifier si c'est 0xAE (code) alors autoriser la transmission
				} while (instructionCode != -1);			/* Receive until STOP/REPEATED START received */
				
				break;
			}
			
			case 1://transmit
			{
				int8_t Ack_status;
		        if(drapeau==1){ //si bon code envoi l'état de PB0
				do
					{   
						Ack_status = I2C_Slave_Transmit(doorStatus);	/* Send data byte */
				
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
