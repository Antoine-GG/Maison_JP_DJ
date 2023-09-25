#define F_CPU 1000000UL

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include "I2C_Master_H_file.h"				/* Include I2C header file */
#include "houseStatus.h"

#define SLAVE1    0x3C ///0011 1100
#define SLAVE2    0x5A ///0101 1010

HouseStatus status;
volatile uint8_t windowState1 = 0; //window1 state from U1
volatile uint8_t windowState2 = 0; //window2 state from U1
volatile uint8_t doorState= 0; //door state from U2
volatile uint8_t doorLockState = 0; //
volatile char keyboardState ='0';

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
uint8_t call_I2cDevice(uint8_t address){
		uint8_t data;
		I2C_Repeated_Start(address | 0b1); // force LMS bit to 1 for read mode
		data = I2C_Read_Ack();
		I2C_Stop();
}

int main() {
	initUSART();
	I2C_Start();
	_delay_ms(500); // Attendre pour laisser les ATmega328P esclaves s'initialiser
	DDRB |= (1 << PB0) | (1 << PB1);	
	
	while (1) {
	
		uint8_t received;
		//CALL U1, window1 and window2 are the 2 last bits as 0000 0012
		received = call_I2cDevice(SLAVE1);
		HouseStatus.window2 = received & 0x01; //Mask the lsb
		HouseStatus.window1 = (received >> 1) & 0x01; // Shift right by 1 and mask with 0x01
		//CALL U2, get the door status;
		received = call_I2cDevice(SLAVE2);
		HouseStatus.door = received & 0x01; //Mask the lsb
		
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