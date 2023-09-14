/*
 * main.c
 *
 * Created: 9/8/2023 3:19:56 PM
 *  Author: J�r�my JJBDD
 */ 

#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}




#define SLAVE_ADDRESS_1 0x30 // Adresse de l'ATmega328P esclave 1
#define SLAVE_ADDRESS_2 0x08 // Adresse de l'ATmega328P esclave 2

volatile uint8_t receivedData = 0;

void i2c_init_slave() {
	TWAR = (SLAVE_ADDRESS_1 << 1); // Configurer l'adresse de l'esclave 1 en mode lecture
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE); // Activer l'interface I2C en mode esclave avec ACK
	sei(); // Activer les interruptions globales
}

int main() {
	i2c_init_slave();

	while (1) {
		// Le code de l'esclave peut rester vide, car la communication est g�r�e par les interruptions
	}

	return 0;
}

ISR(TWI_vect) {
	if ((TWSR & 0xF8) == TW_SR_SLA_ACK) { // R�ception de l'adresse de ma�tre
		TWCR |= (1 << TWINT); // Effacer le drapeau d'interruption
		} else if ((TWSR & 0xF8) == TW_SR_DATA_ACK) { // R�ception de donn�es
		receivedData = TWDR; // Lire les donn�es re�ues
		// Traiter les donn�es re�ues (dans cet exemple, nous ne faisons que les ignorer)
		TWCR |= (1 << TWINT); // Effacer le drapeau d'interruption
	}
}






////////////// CODE A VOIR/TESTER /////////////

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#define SLAVE_ADDRESS_1 0x30 // Adresse de l'ATmega328P esclave 1
#define SLAVE_ADDRESS_2 0x08 // Adresse de l'ATmega328P esclave 2

volatile uint8_t windowState1 = 0; // �tat de la fen�tre de l'esclave 1
volatile uint8_t windowState2 = 0; // �tat de la fen�tre de l'esclave 2

void i2c_init() {
	TWSR = 0; // Pas de division de fr�quence
	TWBR = ((F_CPU / 100000) - 16) / 2; // Calcul de la valeur de TWBR pour une fr�quence I2C de 100 kHz
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Activer l'interface I2C en mode ma�tre
}

void i2c_start() {
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_stop() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
	_delay_ms(1); // Attendre pour s'assurer que l'arr�t est effectu�
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

	while (1) {
		// Demander l'�tat de la fen�tre de l'esclave 1
		i2c_start();
		i2c_write((SLAVE_ADDRESS_1 << 1) | 1); // �crire l'adresse de l'ATmega328P esclave 1 en mode lecture
		windowState1 = i2c_read();
		i2c_stop();
		
		// Demander l'�tat de la fen�tre de l'esclave 2
		i2c_start();
		i2c_write((SLAVE_ADDRESS_2 << 1) | 1); // �crire l'adresse de l'ATmega328P esclave 2 en mode lecture
		windowState2 = i2c_read();
		i2c_stop();
		
		// Traiter les donn�es re�ues et allumer ou �teindre les LED en fonction de l'�tat de la fen�tre
		if (windowState1 == 1) {
			// Allumer la LED 1 (par exemple, en activant une sortie)
			} else {
			// �teindre la LED 1 (par exemple, en d�sactivant une sortie)
		}
		
		if (windowState2 == 1) {
			// Allumer la LED 2 (par exemple, en activant une autre sortie)
			} else {
			// �teindre la LED 2 (par exemple, en d�sactivant une autre sortie)
		}
		
		_delay_ms(1000); // Attendre avant de refaire la demande
	}

	return 0;
}

