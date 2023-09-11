/*
 * main.c
 *
 * Created: 9/8/2023 3:19:56 PM
 *  Author: 2144797
 */ 

#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

/*int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}*/




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
		// Le code de l'esclave peut rester vide, car la communication est gérée par les interruptions
	}

	return 0;
}

ISR(TWI_vect) {
	if ((TWSR & 0xF8) == TW_SR_SLA_ACK) { // Réception de l'adresse de maître
		TWCR |= (1 << TWINT); // Effacer le drapeau d'interruption
		} else if ((TWSR & 0xF8) == TW_SR_DATA_ACK) { // Réception de données
		receivedData = TWDR; // Lire les données reçues
		// Traiter les données reçues (dans cet exemple, nous ne faisons que les ignorer)
		TWCR |= (1 << TWINT); // Effacer le drapeau d'interruption
	}
}
