/*
 * main.c
 *
 * Created: 9/17/2023 5:29:21 PM
 *  Author: Jérémy JBDD
 */ 

/*
#include <xc.h>

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
*/


/////////////////////////////////////////////////////////////////////////
/*
#include <xc.h>
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>


int main(void){

	DDRD |= (1 << PIND0);//PORTD pin0 as INPUT
	DDRC=0xFF;//PORTC as OUTPUT

	int UBRR_Value = 25; // 2400 baud rate

	UBRR0H = (unsigned char) (UBRR_Value >> 8);

	UBRR0L = (unsigned char) UBRR_Value;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	UCSR0C = (1 << USBS0) | (3 << UCSZ00);


	unsigned char receiveData;


	while (1){

		while (! (UCSR0A & (1 << RXC0)) );

		receiveData = UDR0;

		if (receiveData == 0b11110000){

			PORTC ^= (1 << PINC0);

			_delay_ms(220);

		}
		
	}
	
}
*/

////////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

// Définition des broches
#define LED_PIN PB0 // Broche 0 du port B pour la LED

void USART_Init(unsigned int ubrr) {
	// Configuration du registre UBRR pour la vitesse de communication
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	
	// Activation du récepteur et du transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Format de trame : 8 bits de données, 1 bit d'arrêt, pas de parité
	//UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00); // 8 data, 2 stop
}

unsigned char USART_Receive() {
	// Attente de données à recevoir
	while (!(UCSR0A & (1 << RXC0)));
	
	// Récupération des données du tampon
	return UDR0;
}

int main(void) {
	// Initialisation de l'UART à 9600 bauds (même vitesse que l'émetteur)
	USART_Init(103); // 9600 bauds pour une fréquence de 1 MHz
	
	// Configuration de la broche de la LED comme sortie
	DDRB |= (1 << LED_PIN);
	
	while (1) {
		// Attente de données de l'émetteur
		char receivedData = USART_Receive();
		
		// Si 'A' est reçu, allumer la LED
		if (receivedData == 0b00000001) {
			PORTB |= (1 << LED_PIN); // Allumer la LED
			
			// Attendre un moment (vous pouvez ajuster la durée)
			_delay_ms(1000);
			
			PORTB &= ~(1 << LED_PIN); // Éteindre la LED
		}
	}
	
	return 0;
}
