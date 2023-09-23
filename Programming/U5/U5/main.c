/*
 * main.c
 *
 * Created: 9/14/2023 3:46:22 PM
 *  Author: JP Toutant, ChatGPT
 *	Fonctionnement confirmé avec arduino ensuite porté sur avr!
 *	
 */ 


#define F_CPU 1000000UL
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>

#define LATCH_PIN   PC0
#define CLOCK_PIN   PC1
#define DATA_PIN    PC2
#define BUZZ_PIN    PC3
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

#define NOTE_C4   261.63
#define NOTE_E4   329.63
#define NOTE_G4   392.00 


void initIOports(){
	
	// Configure Timer1 for Fast PWM mode
	TCCR1A |= (1 << COM1B1) | (1 << WGM10); // Non-inverted Fast PWM
	TCCR1B |= (1 << WGM12) | (1 << CS11);    // Prescaler 8
	
	// Initialize I/O pins
	DDRB |= (1 << DEBUG_PIN); // Buzzer pin as output
	DDRC |= (1 << BUZZ_PIN);  // Buzzer pin as output
	DDRC |= (1 << LATCH_PIN); // Latch pin as output
	DDRC |= (1 << CLOCK_PIN); // Clock pin as output
	DDRC &= ~(1 << DATA_PIN); // Data pin as input
		
	// Enable pull-up resistor for data pin
	PORTC |= (1 << DATA_PIN);
		
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

void playTone(double frequency, uint16_t duration) {
	uint16_t period = 1000000.0 / (2.0 * frequency); // Calculate the period in microseconds
	uint16_t cycles = duration * 1000 / period;      // Calculate the number of cycles

	for (uint16_t i = 0; i < cycles; i++) {
		// Toggle the buzzer pin to generate a square wave
		PORTC ^= (1 << BUZZ_PIN);
		//_delay_us(period / 2);
		_delay_ms(100);
	}
}

// Function to read data from the shift registers
uint16_t readShiftRegisters() {
	uint16_t data = 0;
	
	// Latch low, fin de la dompe parallele precedente
	PORTC &= ~(1 << LATCH_PIN);
	_delay_us(5); 
	// Latch High, cause les pins du clavier de se décharger dans le registre
	PORTC |= (1 << LATCH_PIN);
	
	// 16 coups de clock pour acheminer le tapis roulant du serial vers notre pin data
	for (int i = 0; i < 16; i++) {
		// Pulse the clock pin to shift in the next bit
		PORTC |= (1 << CLOCK_PIN);
		_delay_us(100); // Adjust delay as needed based on your clock frequency
		PORTC &= ~(1 << CLOCK_PIN);
		
		// Read the data pin and store the bit
		data |= ((PINC & (1 << DATA_PIN)) >> DATA_PIN) << i;
	}
	
	
	return data;
}

int main() {

	while (1) {
		// Read data from shift registers
		uint16_t shiftRegisterData = readShiftRegisters();
		switch (shiftRegisterData)
		{
		case KEY_0:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB |= (1 << DEBUG_PIN);
			transmitByte('0'); 
			break;
		case KEY_1:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB &= ~(1 << DEBUG_PIN);
			transmitByte('1'); 
			break;
		case KEY_2:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB |= (1 << DEBUG_PIN);
			transmitByte('2'); 
			break;
		case KEY_3:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB &= ~(1 << DEBUG_PIN);
			transmitByte('3'); 
			break;
		case KEY_4:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB |= (1 << DEBUG_PIN);
			transmitByte('4'); 
			break;
		case KEY_5:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB &= ~(1 << DEBUG_PIN);
			transmitByte('5'); 
			break;
		case KEY_6:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB |= (1 << DEBUG_PIN);
			transmitByte('6'); 
			break;
		case KEY_7:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB &= ~(1 << DEBUG_PIN);
			transmitByte('7'); 
			break;
		case KEY_8:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB |= (1 << DEBUG_PIN);
			transmitByte('8'); 
			break;
		case KEY_9:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB &= ~(1 << DEBUG_PIN);
			transmitByte('9'); 
			break;
		case KEY_STAR:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB |= (1 << DEBUG_PIN);
			transmitByte('*'); 
			break;
		case KEY_SHARP:
			playTone(NOTE_C4, 12);
			playTone(NOTE_E4, 12);
			playTone(NOTE_G4, 12);
			PORTB &= ~(1 << DEBUG_PIN);
			transmitByte('#'); 
			break;
		default:
			break;
		}
		// Process and use the received data as needed
		// You can print it, send it to other devices, or perform any desired operations
		
		// Delay between data reads (adjust as needed)
		_delay_ms(50); // We need a responsive rate
	}
	
	return 0;
}