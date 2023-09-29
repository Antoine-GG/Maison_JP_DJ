/*
 * main.c
 *
 * Created: 9/29/2023 2:41:11 PM
 *  Author: 0279137
 */ 
#define F_CPU 8000000UL

#include <xc.h>
#include<avr/io.h>
#include<util/delay.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<inttypes.h>
#include <avr/sfr_defs.h>
#include <avr/power.h>
#include <util/delay.h>

void initUSART(void) {
	UBRR0H = 0;              /* baud rate  */
	UBRR0L = 103;           /* 9600 */
	UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    /* Activer emission et reception  USART */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit, valeur au reset*/
}

void transmitByte(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Attendre que le buffer de transmission soit vide */  	UDR0 = data;                      /* envoyer la donnée */
}

uint8_t receiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); /* Attendre que le buffer de réception soit plein */
	return UDR0;                                /* retourner la valeur lue */
}

void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}


int main(void)
{	
	uint8_t character = 30;
	initUSART();
    while(1)
    {
		transmitByte('character');
		_delay_ms(250);
		character++;
		if(character >= 0xFF){
			character = 30;
		}
		
        //TODO:: Please write your application code 
    }
}