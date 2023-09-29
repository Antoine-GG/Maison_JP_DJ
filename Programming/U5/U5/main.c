/*
 * main.c
 *
 * Created: 9/14/2023 3:46:22 PM
 *  Author: JP Toutant, ChatGPT
 *	Prototype arduino ensuite port sur avr
 *	
 */ 


#define F_CPU 1000000UL
#include <xc.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include "BuzzerSounds.h"


#define LATCH_PIN   PC0
#define CLOCK_PIN   PC1
#define DATA_PIN    PC2
#define MOSI_PIN    PB3     // MOSI: Master Out Slave In
#define MISO_PIN    PB4     // MISO: Master In Slave Out
#define SCK_PIN     PB5     // SCK: Serial Clock
#define SS_PIN      PB2     // SS: Slave Select (Chip Select)
#define DEBUG_PIN   PB0     // SS: Slave Select (Chip Select)

#define KEY_STAR  0x0008
#define KEY_7	  0x0001
#define KEY_4     0x0010
#define KEY_1     0x0020
#define KEY_0     0x0040
#define KEY_8     0x0080
#define KEY_5     0x0800
#define KEY_2     0x0100
#define KEY_SHARP 0x1000
#define KEY_9     0x2000
#define KEY_6     0x4000
#define KEY_3     0x8000

void initIOports(){
	// Initialize I/O pins
	DDRB |= (1 << PB1); // Buzzer pin as output
	DDRC |= (1 << LATCH_PIN); // Latch pin as output
	DDRC |= (1 << CLOCK_PIN); // Clock pin as output
	DDRC &= ~(1 << DATA_PIN); // Data pin as input
	//MISO as output, all others input
	DDRB |= (1 << MISO_PIN);
	//SPI inputs
	DDRB &= ~(1 << SS_PIN);
	DDRB &= ~(1 << MOSI_PIN);
	DDRB &= ~(1 << SCK_PIN);
	
	// Enable pull-up resistor for data pin
	PORTC |= (1 << DATA_PIN);
		
}
void SPI_SlaveInit(void)
{
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{	
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	;
	/* Return Data Register */
	return SPDR;
}


//initialiser UART
void initUSART(void) {
	UBRR0H = 0;              /* baud rate  */
	UBRR0L = 0x0C;           /* 9600 */
	UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    /* Activer emission et reception  USART */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit, valeur au reset*/
}

//transmettre un caract�re
void SPI_SlaveTransmit(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Attendre que le buffer de transmission soit vide */
	UDR0 = data;                      /* envoyer la donn�e */
}

//recevoir un caract�re
uint8_t receiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); /* Attendre que le buffer de r�ception soit plein */
	return UDR0;                                /* retourner la valeur lue */
}

//envoyer une chaine de caract�res
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		SPI_SlaveTransmit(myString[i]);
		i++;
	}
}
//SPI CALLS


// Function to read data from the shift registers
uint16_t readShiftRegisters() {
	uint16_t data = 0;
	
	// Latch low, fin de la dompe parallele precedente
	PORTC |= (1 << LATCH_PIN);
	_delay_us(15);
	PORTC &= ~(1 << LATCH_PIN);
	_delay_us(15);
	PORTC &= ~(1 << LATCH_PIN);
	_delay_us(15);
	// 16 clocks pour acheminer 16bits serial vers notre pin data
	for (int i = 0; i < 16; i++) {
		// Pulse the clock pin to shift in the next bit
		PORTC |= (1 << CLOCK_PIN);
		_delay_us(15); // Adjust delay as needed based on your clock frequency
		PORTC &= ~(1 << CLOCK_PIN);
		
		// Read the data pin and store the bit
		data |= (PINC & (1 << DATA_PIN)) << i;
	}
	return data;
}

int main() {
	uint8_t feedback =0;
	initIOports();
	SPI_SlaveInit();
	//sound_fail();
	//sound_input();
	//sound_success();
	while (1) {

		// Read data from shift registers
		uint16_t shiftRegisterData = readShiftRegisters();
		switch (shiftRegisterData)
		{
		case 0:
			break;
		case KEY_0:
			PORTB |= (1 << DEBUG_PIN);
			SPDR = '0';
			break;
		case KEY_1:
			PORTB &= ~(1 << DEBUG_PIN);
			SPDR = '1';
			break;
		case KEY_2:
			PORTB |= (1 << DEBUG_PIN);
			SPDR = '2';
			break;
		case KEY_3:
			PORTB &= ~(1 << DEBUG_PIN);
			SPDR = '3';
			break;
		case KEY_4:
			PORTB |= (1 << DEBUG_PIN);
			SPDR = '4'; 
			break;
		case KEY_5:
			PORTB &= ~(1 << DEBUG_PIN);
			SPDR = '5'; 
			break;
		case KEY_6:
			PORTB |= (1 << DEBUG_PIN);
			SPDR = '6';  
			break;
		case KEY_7:
			PORTB &= ~(1 << DEBUG_PIN);
			SPDR = '7';  
			break;
		case KEY_8:
			PORTB |= (1 << DEBUG_PIN);
			SPDR = '8'; 
			break;
		case KEY_9:
			PORTB &= ~(1 << DEBUG_PIN);
			SPDR = '9'; 
			break;
		case KEY_STAR:
			PORTB |= (1 << DEBUG_PIN);
			SPDR = 'A';
			break;
		case KEY_SHARP:
			PORTB &= ~(1 << DEBUG_PIN);
			SPDR = 'B';
			break;
		default:
			SPDR = 'X';
			break;
		}
		feedback = SPI_SlaveReceive();
		switch (feedback)
		{
		case 0xF0:
			sound_input();	 
			break;
		case 0xE0:
			sound_fail();	 
			break;
		case 0xD0:
			sound_success();	 
			break;		
		default:
			break;
		}
		
		_delay_ms(1); // We need a responsive rate
	}
	return 0;
}
