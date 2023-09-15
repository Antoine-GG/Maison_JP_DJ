#define F_CPU 1000000UL 

#include <xc.h>
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/>

#define SLAVE_ADDRESS_1 0x30 // Adresse de l'ATmega328P esclave 1
#define SLAVE_ADDRESS_2 0x08 // Adresse de l'ATmega328P esclave 2
#define MAX_I2C_RETRIES 3

volatile uint8_t windowState1 = 0; // État de la fenêtre de l'esclave 1
volatile uint8_t windowState2 = 0; // État de la fenêtre de l'esclave 2
int iteration =0;

void i2c_init() {
	// SDA (PC4) and SCL (PC5) are configured as inputs with pull-up resistors.
	PORTC |= (1 << PC4) | (1 << PC5); // Enable pull-up resistors on SDA and SCL
	
	TWSR = 0; // No clock prescaling
	TWBR = 12; // Set TWI bit rate to achieve 100 kHz I2C frequency
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Enable I2C, generate START condition, and clear interrupt flag
	sei(); // Enable global interrupts
}

void i2c_start() {
	uint8_t retries = 0;
	
	while (1) {
		// Initiate START condition
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
		
		// Wait for TWINT flag to be set (START condition transmitted)
		//if ((TWCR & (1 << TWINT))){
		//	break;
		//}
		
		// Check for START condition success
		if ((TWSR & 0xF8) == TW_START) {
			break; // START condition successful, exit loop
		}
		
		// If START condition failed, increment retries and try again
		retries++;
	}
}

void i2c_stop() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
	_delay_ms(1); // Attendre pour s'assurer que l'arrêt est effectué
}

void i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read() {
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

int main() {
	
	i2c_init();
	_delay_ms(500); // Attendre pour laisser les ATmega328P esclaves s'initialiser
	DDRB |= (1 << PB0) | (1 << PB1);

	while (1) {
		// Demander l'état de la fenêtre de l'esclave 1
		i2c_start();
		//i2c_write(0x30); // Écrire l'adresse de l'ATmega328P esclave 1 en mode lecture
		//windowState1 = i2c_read();
		i2c_stop();
		
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
		
		
			PORTB |= (1 << PB0);	// Allumer la LED 2 
			_delay_ms(500);
			
			PORTB &= ~(1 << PB0);	// Éteindre la LED 2 
			
		
		
		_delay_ms(500); // Attendre avant de refaire la demande
		
	}

	return 0;
}

