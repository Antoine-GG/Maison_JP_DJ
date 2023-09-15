#include <xc.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#define F_CPU 1000000UL
#include <util/delay.h>

#define SLAVE_ADDRESS 0x30	//addresse i2c de U1

volatile uint8_t window1 = 0;
volatile uint8_t window2 = 0;

void i2c_init_slave(uint8_t slave_address) { //Initialize I2C in slave mode
	TWAR = (slave_address << 1); //Configure l'adresse de l'esclave en mode lecture
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE); // Activer l'interface I2C en mode esclave avec ACK et interruptions
}

void i2c_start() {
	//Attendre jusqu'à ce que l'adresse soit reçue et que le bus I2C soit occupé
	while (!(TWCR & (1 << TWINT))) {
	// Peut être utilisé pour effectuer des opérations spécifiques à l'esclave si nécessaire
	}
}

void i2c_stop() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
}

void i2c_write(uint8_t data) {		//Permet d'écrire des données dans le registre de données I2C (TWDR)
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
}

void i2c_ack() {		//Envoie un acknowledge pour dire : Slave prêt a recevoir données 
	TWCR |= (1 << TWEA) | (1 << TWINT);
}

void i2c_nack() {		//Attend que les données soient disponibles, puis les lit et les renvoie
	TWCR = (1 << TWINT) | (1 << TWEN);
}

void initIOports(){

	// Set PC4 and PC5 as output pin for debug
	DDRC |= (1 << PC3);
	DDRC |= (1 << PC2);

	// Set PD6 and PD7 as input pins
	DDRD &= ~(1 << PD6);
	DDRD &= ~(1 << PD7);


	// Enable pull-up resistors on PD3 and PD4
	//PORTD |= (1 << 3);
	//PORTD |= (1 << 4);
}

uint8_t i2c_read() {
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT))) {
		//Peut être utilisé pour effectuer des opérations spécifiques à l'esclave si nécessaire
	}
	return TWDR;		//donnée que l'esclave a reçue du maître
}


ISR(TWI_vect) {		//réception de données et les stocke dans les variables window1 et window2
	uint8_t status = TWSR & 0xF8;
	uint8_t receivedData;
	
	switch (status) {
		case TW_SR_SLA_ACK: // Adresse Maitre reçue
			i2c_ack();
			break;

		case TW_SR_DATA_ACK: // Données reçues
			receivedData= TWDR;
		
			// stocker les données reçues
			window1 = receivedData & (1 << 0);
			window2 = receivedData & (1 << 1);

			i2c_ack(); // Envoyer un ACK pour indiquer que nous sommes prêts à recevoir plus de données
			break;

		default:
			i2c_nack(); // Envoyer un NACK en cas d'erreur ou de condition non gérée
			break;
	}
}


int main(void) {
	i2c_init_slave(SLAVE_ADDRESS);
	initIOports();
	
	sei(); //Activer les interruptions globales
	
	while (1) {
		//La communication I2C est gérée par les interruptions
		
		//read the state of the doors at pin 3 and 4
		window1 = PIND & (1 << PD6);
		window2 = PIND & (1 << PD7);
		//if the door1 is open set PC5 pin to high otherwise set it to low
		if(window1 == 0){
			PORTC |= (1 << PC3);
			}else{
			PORTC &= ~(1 << PC3);
		}
		if(window2 == 0){
			PORTC |= (1 << PC2);
			}else{
			PORTC &= ~(1 << PC2);
		}
		_delay_ms(100);
	}
	
	return 0;
}