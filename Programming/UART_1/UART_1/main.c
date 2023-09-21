/*
 * main.c
 *
 * Created: 9/17/2023 3:45:54 PM
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


///////////////////////////////////////////////////////////////////////////////////////////
/*
#include <xc.h>
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>


int main(void){

	DDRD |= 1 << PIND1;//pin1 of portD as OUTPUT
	DDRD &= ~(1 << PIND0);//pin0 of portD as INPUT
	PORTD |= 1 << PIND0;

	int UBBRValue = 25;//AS described before setting baud rate

	//Put the upper part of the baud number here (bits 8 to 11)
	UBRR0H = (unsigned char) (UBBRValue >> 8);


	//Put the remaining part of the baud number her
	UBRR0L = (unsigned char) UBBRValue;


	//Enable the receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);


	//Set 2 stop bits and data bit length is 8-bit
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);


	while (1){

		if (bit_is_clear(PINC,0))//once button is pressed
		
		{

			while (! (UCSR0A & (1 << UDRE0)) );

			{
				UDR0 = 0b11110000;//once transmitter is ready sent eight bit data
			}

			// Get that data outa here
			_delay_ms(220);


		}
	}
}
*/

//////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

// Définition des broches
#define BUTTON_PIN PD2 // Broche 2 du port D pour le bouton

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

void USART_Transmit(unsigned char data) {
	// Attente que le tampon de transmission soit vide
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Écriture des données dans le tampon, ce qui les envoie
	UDR0 = data;
}

int main(void) {
	// Initialisation de l'UART à 9600 bauds (vous pouvez ajuster la vitesse si nécessaire)
	USART_Init(103); // 9600 bauds pour une fréquence de 1 MHz
	
	// Configuration de la broche du bouton comme entrée
	DDRD &= ~(1 << BUTTON_PIN);
	// Activation de la résistance de tirage pour le bouton
	PORTD |= (1 << BUTTON_PIN);
	
	while (1) {
		// Vérification si le bouton est pressé
		if (!(PIND & (1 << BUTTON_PIN))) {
			// Envoyer un caractère ('A' dans cet exemple)
			USART_Transmit(0b00000001);
			
			// Attendre un peu pour éviter la détection multiple
			_delay_ms(500);
		}
	}
	
	return 0;
}
