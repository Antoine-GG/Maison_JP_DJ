#define F_CPU 1000000UL

#include <xc.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <uart.h>

#define SLAVE1    0x33
#define SLAVE2    0x22

volatile uint8_t windowState1 = 0; //window1 state from U1
volatile uint8_t windowState2 = 0; //window2 state from U1
volatile uint8_t doorState= 0; //door state from U2
volatile uint8_t doorLockState = 0; //

//init UART
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

void initI2C(void) {
	TWBR = 12;                               /* set bit rate, */
	/* 8MHz / (16+2*TWBR*1) ~= 100kHz */
	TWCR |= (1 << TWEN);                                       /* enable */
}

void i2cWaitForComplete(void) {
	loop_until_bit_is_set(TWCR, TWINT);
}

void i2cStart(void) {
	TWCR = (1 <<TWINT) | (1 <<TWEN) | (1 <<TWSTA);
	i2cWaitForComplete();
}

void i2cStop(void) {
	TWCR = (1 <<TWINT) | (1 <<TWEN) | (1 <<TWSTO);
}

uint8_t i2cReadAck(void) {
	TWCR =(1 <<TWINT) | (1 <<TWEN) | (1 <<TWEA);
	i2cWaitForComplete();
	return (TWDR);
}

uint8_t i2cReadNoAck(void) {
	TWCR = (1 <<TWINT) | (1 <<TWEN);
	i2cWaitForComplete();
	return (TWDR);
}

void i2cSend(uint8_t data) {
	TWDR = data;
	TWCR = (1 <<TWINT) | (1 <<TWEN);                  /* init and enable */
	i2cWaitForComplete();
}

int main() {
	
	//clock_prescale_set(clock_div_1);
	initI2C();
	
	while (1) {
		// Demander l'état de la fenêtre de l'esclave 1
		i2cStart();
		i2cSend(SLAVE1);
		i2cStart();
		windowState1 = i2cReadNoAck();
		i2cStop();
		
		
		//transmitByte(windowState1);
		// Demander l'état de la fenêtre de l'esclave 2
		//i2c_start();
		//i2c_write((SLAVE_ADDRESS_2 << 1) | 1); // Écrire l'adresse de l'ATmega328P esclave 2 en mode lecture
		//windowState2 = i2c_read();
		//i2c_stop();
		
		// Traiter les données reçues et allumer ou éteindre les LED en fonction de l'état de la fenêtre
		//if (windowState1 == 1) {
		//	PORTB |= (1 << PB1);	// Allumer la LED 1
		//
		//	} else {
		//	PORTB &= ~(1 << PB1);	// Éteindre la LED 1
		//}
		//
		//if (windowState2 == 1) {
		//	PORTB |= (1 << PB0);	// Allumer la LED 2
		//
		//	} else {
		//	PORTB &= ~(1 << PB0);	// Éteindre la LED 2
		//
		//}
		//
		_delay_ms(50); // Attendre avant de refaire la demande
	}

	return 0;
}

	//DDRB |= (1 << PB0) | (1 << PB1);


		// Demander l'état de la fenêtre de l'esclave 1
		//i2c_start();
		//i2c_write((SLAVE_ADDRESS_1 << 1) | 1); // Écrire l'adresse de l'ATmega328P esclave 1 en mode lecture
		//windowState1 = i2c_read();
		//i2c_stop();
		
		// Demander l'état de la fenêtre de l'esclave 2
		//i2c_start();
		//i2c_write((SLAVE_ADDRESS_2 << 1) | 1); // Écrire l'adresse de l'ATmega328P esclave 2 en mode lecture
		//windowState2 = i2c_read();
		//i2c_stop();
		
		// Traiter les données reçues et allumer ou éteindre les LED en fonction de l'état de la fenêtre
		//if (windowState1 == 1) {
		//	PORTB |= (1 << PB1);	// Allumer la LED 1 
			
		//	} else {
		//	PORTB &= ~(1 << PB1);	// Éteindre la LED 1
		//}
		
		//if (windowState2 == 1) {
		//	PORTB |= (1 << PB0);	// Allumer la LED 2 
			
		//	} else {
		//	PORTB &= ~(1 << PB0);	// Éteindre la LED 2 
		
		//_delay_ms(1000); // Attendre avant de refaire la demande
	//}

	//return 0;

