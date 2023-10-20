/*
 * main.c
 *
 * Created: 9/20/2023 7:20:41 AM
 *  Author: JP Toutant
 */ 

#define F_CPU 8000000UL							/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>								/* Include AVR std. library file */
#include <util/delay.h>							/* Include inbuilt defined Delay header file */
#include <stdio.h>								/* Include standard I/O header file */
#include <string.h>								/* Include string header file */
#include <avr/sfr_defs.h>
#include "I2C_Slave_H_File.h"					/* Include I2C slave header file */

#define Slave_Address			0x5A

int main(void)
{
	int drapeau=0;
	int8_t count = 0;
	uint8_t valeur=0b00000000;
	DDRB |=(1<<PB1);//PB1 en sortie  LED temoin de porte
	PORTD|=(1<<PD7);//Resistance de Rappel activé pour le Bouton Poussoir PORTE
	
	I2C_Slave_Init(Slave_Address);
	
	while (1)
	{
		//Afficher l'etat de la porte sur la LED PB1
		if (bit_is_clear(PIND,PD7))
		{
			PORTB |=(1<<PB1) ; //allumer le temoin de la porte
		} else
		{
			PORTB &=~(1<<PB1) ; //eteindre le temoin de la porte
		}
		//lire trame i2c
		switch(I2C_Slave_Listen())				/* Check for any SLA+W or SLA+R */
		{
			case 0://receive
			{
				do
				{
					
					count = I2C_Slave_Receive();/* Receive data byte*/
					if(count==0x35) drapeau=1;  // vérifier si c'est 0x35 (code) alors autoriser la transmission
					
					} while (count != -1);			/* Receive until STOP/REPEATED START received */
					
					break;
				}
				
				case 1://transmit
				{
					int8_t Ack_status;
					if(drapeau==1){ //si bon code envoi l'état de PD7
						do
						{   valeur=(PIND & 0b10000000);  //valeur de PD7
							Ack_status = I2C_Slave_Transmit(valeur);	/* Send data byte */
							
							} while (Ack_status == 0);		/* Send until Acknowledgment is received */
							
							drapeau=0;//reset pour lire une autre donnée
						}
						break;
					}
					default:
					break;
				}
			}
		}