/*
 * main.c
 *
 * Created: 9/17/2023 5:29:21 PM
 *  Author: J�r�my JBDD
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

// D�finition des broches
#define LED_PIN PB0 // Broche 0 du port B pour la LED

void USART_Init(unsigned int ubrr) {
	// Configuration du registre UBRR pour la vitesse de communication
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	
	// Activation du r�cepteur et du transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Format de trame : 8 bits de donn�es, 1 bit d'arr�t, pas de parit�
	//UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00); // 8 data, 2 stop
}

unsigned char USART_Receive() {
	// Attente de donn�es � recevoir
	while (!(UCSR0A & (1 << RXC0)));
	
	// R�cup�ration des donn�es du tampon
	return UDR0;
}

int main(void) {
	// Initialisation de l'UART � 9600 bauds (m�me vitesse que l'�metteur)
	USART_Init(103); // 9600 bauds pour une fr�quence de 1 MHz
	
	// Configuration de la broche de la LED comme sortie
	DDRB |= (1 << LED_PIN);
	
	while (1) {
		// Attente de donn�es de l'�metteur
		char receivedData = USART_Receive();
		
		// Si 'A' est re�u, allumer la LED
		if (receivedData == 0b00000001) {
			PORTB |= (1 << LED_PIN); // Allumer la LED
			
			// Attendre un moment (vous pouvez ajuster la dur�e)
			_delay_ms(1000);
			
			PORTB &= ~(1 << LED_PIN); // �teindre la LED
		}
	}
	
	return 0;
}
