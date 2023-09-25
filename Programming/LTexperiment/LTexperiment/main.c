/*
 * main.c
 *
 * Created: 9/21/2023 12:22:12 AM
 *  Author: Utilisateur
 */ 
#define F_CPU 1000000UL
#include <xc.h>
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "I2C_Master_H_file.h"

#define SLAVE_ADDRESS   0x54
#define	SLAVE_ADDRESS_R 0x55

//io pins setup
void initIO(void){
	DDRB |= (1 << PB0);
	
}
//initialiser UART
void initUSART(void) {
	UBRR0H = 0;              /* baud rate  */
	UBRR0L = 0x0C;           /* 9600 */
	UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    /* Activer emission et reception  USART */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit, valeur au reset*/
}
//transmettre un caractère
void transmitByte(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Attendre que le buffer de transmission soit vide */
	UDR0 = data;                      /* envoyer la donnée */
}
//recevoir un caractère
uint8_t receiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); /* Attendre que le buffer de réception soit plein */
	return UDR0;                                /* retourner la valeur lue */
}
//envoyer une chaine de caractères
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}

int main(void) {
	_delay_ms(500);
	initIO();
	uint8_t received;
	I2C_Init();
	
	while(1)
	{
		I2C_Start_Wait(SLAVE_ADDRESS);
		_delay_ms(5);
		
		I2C_Write(0x25);
		_delay_ms(5);
		
		I2C_Repeated_Start(SLAVE_ADDRESS_R);
		_delay_ms(5);
		
		received = I2C_Read_Nack();
		_delay_ms(5);
		
		I2C_Stop();
		if(received == 1){
			PORTB |= (1 << PB0);
		}
		else{
			PORTB &= ~(1 << PB0);
		}
			
		_delay_ms(30);
	}
}