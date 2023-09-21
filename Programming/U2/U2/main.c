/*
 * main.c
 *
 * Created: 9/20/2023 7:20:41 AM
 *  Author: JP Toutant
 */ 
#define F_CPU 1000000UL
#include <xc.h>
#include <avr/io.h>
#include <util/twi.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <util/delay.h>

#define SLAVE_ADDRESS 0x20


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

void i2c_initSlave(void){
	TWAR = (SLAVE_ADDRESS<<1);
	TWCR = (1<<TWEA) | (1<<TWEN);
}

void i2c_listen(void){
	while (!(TWCR & (1<<TWINT)));
}

void i2c_init(void){
	TWSR = 0;  //We dont want to prescale
	TWBR = ((F_CPU / 100000UL)-16)/2; //formula for bitrate
}

void i2c_start(void){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR &(1<<TWINT)));
}

void i2c_stop(void){
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

void i2c_write(uint8_t data){
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}



int main(void)
{
	uint8_t data = 'A';
	i2c_initSlave();
    while(1)
    {
       i2c_listen();
	   if((TWSR & 0xF8) == TW_SR_SLA_ACK)
	   {
		   i2c_write(data);
	   }
	   
    }
}
