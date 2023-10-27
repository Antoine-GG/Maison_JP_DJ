/*
 * ATmega32_Slave.c
 * http://www.electronicwings.com
 *
 *
 *LE Slave 1 //addresse 0x3C     pour les 2 fenetres
 *Le Slave attend la reception du code 0x25 du Master
 *le code recu est envoyé sur le PORTD ( mettre 8 led pour voir le code)
 */ 


#define F_CPU 8000000UL							/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>								/* Include AVR std. library file */
#include <util/delay.h>							/* Include inbuilt defined Delay header file */
#include <stdio.h>								/* Include standard I/O header file */
#include <string.h>								/* Include string header file */
#include <avr/sfr_defs.h>
#include "I2C_Slave_H_File.h"					/* Include I2C slave header file */

#define Slave_Address			0x3C

int main(void)
{
	int drapeau=0;
	int8_t count = 0;
	uint8_t valeur=0b00000000;
	
	DDRB |=(1<<PB2);//temoin fenetre 2
	DDRB |=(1<<PB1);//temoin fenetre 1
	
	//PORTD|=(1<<PD6);//Resistance de Rappel activé pour le Bouton Poussoir FENETRE 1
	//PORTD|=(1<<PD7);//Resistance de Rappel activé pour le Bouton Poussoir FENETRE 2
	
	
	I2C_Slave_Init(Slave_Address);
		
	while (1)
	{
		//afficher les etats des fenetres sur les leds
		if (bit_is_clear(PIND,PD7))
		{
			PORTB |=(1<<PB2) ; //allumer le temoin de la fenetre 2
		} else
		{
			PORTB &=~(1<<PB2) ; //eteindre le temoin de la fenetre 2
		}
		
		if (bit_is_clear(PIND,PD6))
		{
			PORTB |=(1<<PB1) ; //allumer le temoin de la fenetre 1
		} else
		{
			PORTB &=~(1<<PB1) ; //eteindre le temoin de la fenetre 1
		}
		switch(I2C_Slave_Listen())				/* Check for any SLA+W or SLA+R */
		{
			case 0://receive
			{
				do
				{
	
					count = I2C_Slave_Receive();/* Receive data byte*/
					if(count==0x25) drapeau=1;  // vérifier si c'est 0x25 (code) alors autoriser la transmission
					PORTD=count;
				} while (count != -1);			/* Receive until STOP/REPEATED START received */
				
				break;
			}
			
			case 1://transmit
			{
				int8_t Ack_status;
		        if(drapeau==1){ //si bon code envoi l'état de PD7 et PD6
				do
					{   valeur=(PIND & 0b11000000);  //valeur de PD7, et PD6
						Ack_status = I2C_Slave_Transmit(valeur);	/* Send data byte */
				
					} while (Ack_status == 0);		/* Send until Acknowledgment is received */
				
					drapeau=0;//reset pour lire une autre donnée
				}
				break;
			}
			case 2: //
			break;
			default:
				break;
		}
	}
}

