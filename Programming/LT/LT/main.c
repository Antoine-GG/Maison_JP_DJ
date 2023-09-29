// Authors:  Idir Chahed, JP Toutant, Jeremy Diaphrase Lachappelle, ChatGPT
// Date: 2023-9-20

#define F_CPU 1000000UL

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include "I2C_Master_H_file.h"				/* Include I2C header file */
#include "houseStatus.h"

#define SLAVE1    0x3C ///0011 1100
#define SLAVE2    0x5A ///0101 1010
#define SLAVE3    2 //
#define SLAVE4    3 //
#define SLAVE5    4 //

HouseStatus status;

//SPI
void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output SS,  */
	DDRB = (1<<PB3)|(1<<PB5)|(1<<PB2);
	// set Port C pins for 3 ss	
	DDRD = (1<<SLAVE3)|(1<<SLAVE4)|(1<<SLAVE5);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
void SPI_MasterTransmit(char cData, uint8_t ss)
{
	/* Set SS low */
	PORTD &= ~(1<<ss);
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));

	//return SPDR;
}
uint8_t SPI_MasterReceive(uint8_t ss)
{
	/* Set SS low */
	PORTB &= ~(1<<ss);
	/* Wait for reception complete */
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	/* Return Data Register */
	return SPDR;
}
uint8_t pollSpiDevice(uint8_t ss){
	uint8_t data;
	SPI_MasterTransmit(0xAE, ss);
	_delay_ms(1);
	data = SPI_MasterReceive(ss);
	_delay_ms(1);
	return data;
}
void SetSpiDevice(uint8_t status, uint8_t ss){
	SPI_MasterTransmit(status, ss);
	_delay_ms(1);
	return;
}

//init UART
void initUSART(void) {
	UBRR0H = 0;              /* baud rate  */
	UBRR0L = 0x0C;           /* 9600 */
	UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    /* Activer emission et reception  USART */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit, valeur au reset*/
}
//transmettre un caractere
void transmitByte(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Attendre que le buffer de transmission soit vide */
	UDR0 = data;                      /* envoyer la donn�e */
}
//recevoir un caractere
uint8_t receiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); /* Attendre que le buffer de r�ception soit plein */
	return UDR0;                                /* retourner la valeur lue */
}
//envoyer une chaine de caracteres
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}
//i2c
uint8_t poll_I2cDevice(uint8_t address){
		uint8_t data;
		I2C_Start_Wait(address); // wait for ACK signal
		_delay_ms(5);

		I2C_Write(0x25); //numero de l'instruction attendu par slave
		_delay_ms(5);

		I2C_Repeated_Start(address | 0b00000001); // force Least Meaningful bit to 1 for read mode
		_delay_ms(5);

		data = I2C_Read_Ack();
		_delay_ms(5);
		
		I2C_Stop();
		return data;
}

int main() {
	uint8_t received;
	SPI_MasterInit();
	initUSART();
	I2C_Init();
	_delay_ms(500); // Attendre pour laisser les ATmega328P esclaves s'initialiser
	
	while (1) {
		received = 0;
		
		//i2c calls
		//POLL U1, window1 and window2 are the 2 last bits as 0000 0012
		received = poll_I2cDevice(SLAVE1);
		status.window2 = received & 0x01; //Mask the lsb
		status.window1 = (received >> 1) & 0x01; // Shift right by 1 and mask with 0x01
		//POLL U2, get the door status;
		received = poll_I2cDevice(SLAVE2);
		status.door = received & 0x01; //Mask the lsb
		//spi calls
		//Set U3 door lock status
		SetSpiDevice(status.lock, SLAVE3);
		//Set U4 light status
		SetSpiDevice(status.light, SLAVE4);
		//POLL U5, get the keyboard char
		status.keyboardChar = pollSpiDevice(SLAVE5);
		_delay_ms(50); // Attendre avant de refaire la demande
	}

	return 0;
}