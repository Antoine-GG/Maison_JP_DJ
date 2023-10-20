/*
 * ATmega16_Master.c
 * http://www.electronicwings.com
 *
 *Le Master envoi le code 0x25 au Slave pour lui dire qu'il doit lui envoyer l'état des Bouton poussoirs relié sur PC2, Pc1 et PC0 (capteur PORTE, FENETRE 1, FENETRE 2.
 *et il passe en mode lecture pour attendre la donnée.
 * En plus Le master va lire la température fournie par le DS1621
 */ 

#define F_CPU 8000000UL						/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>							/* Include AVR std. library file */
#include <util/delay.h>						/* Include inbuilt defined Delay header file */
#include <stdio.h>							/* Include standard I/O header file */
#include <string.h>							/* Include string header file */
#include <avr/sfr_defs.h>
#include "I2C_Master_H_file.h"				/* Include I2C header file */

//--Adresse I2C et commandes----------------------------------------------------------------------
#define Slave_Write_Address1		0x3C     //Atemga 328P Slave Ecriture Slave 1 Fenetres 
#define Slave_Read_Address1		0x3D    //Atemga 328P Slave Lecture  Slave 1  fenetres 
#define Slave_Write_Address2		0x5A     //Atemga 328P Slave Ecriture Slave 2 Porte
#define Slave_Read_Address2		0x5B    //Atemga 328P Slave Lecture  Slave 1  Porte
#define TEMP_W		0x90                 //Adresse d'écriture du capteur de température DS1621
#define TEMP_R		0x91                 //Adresse de lecture du capteur de température DS1621
#define read_temperature 0xAA            //Code de lecture de la température
#define access_config 0xAC               //Registre de contrôle
#define start_conversion 0xEE            //Ordre de conversion

//fonctions UART---------------------------------------------------------------------------------
void initUSART(void) {
	UBRR0H = 0;              // baud rate  
	UBRR0L = 103;           // 9600 
	UCSR0A |= (1 << U2X0);      // mode asynchrone double vitesse 
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    // Activer emission et reception  USART 
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   // 8 data bits, 1 stop bit, valeur au reset
}
void transmitByte(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); // Attendre que le buffer de transmission soit vide 
	UDR0 = data;                      // envoyer la donnée 
}
uint8_t receiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); // Attendre que le buffer de réception soit plein 
	return UDR0;                                // retourner la valeur lue 
}
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}
//-------------------------------------------------------------------------------------------------------

int main()
{  
   char valFenetre;
   char valPorte;
   char aff[6];
   double temperature;
   int8_t tmp_msb,tmp_lsb;
  // declarations des ports utiliser pour temoins des capteurs
   DDRC|=(1<<PC0)|(1<<PC1)|(1<<PC2);
   PORTC &=~(1<<PC0); //Temoin Porte
   PORTC &=~(1<<PC1); //Temoin Fenetre 1
   PORTC &=~(1<<PC2); // Temoin Fenetre 2
 
	I2C_Init();								// Initialize I2C 
	initUSART();                            //Initialiser UART
    
	// DS1621 initialisation,début------
	
    I2C_Start_Wait(TEMP_W);  //Start I2C communication with SLA+W 
	 _delay_ms(5);
    I2C_Write(access_config);
	 _delay_ms(5);
    I2C_Write(0x03);    // execution en mode ordre de conversion
	 _delay_ms(5);
   	I2C_Stop();
   	 _delay_ms(5);
	///-----------------------------------	
	
	while (1)
	{
	// communication avec le Atmega328P Slave 1 Fenetres I2C -----------------------------------------------------------
		I2C_Start_Wait(Slave_Write_Address1);// Start I2C communication with SLA+W 
		_delay_ms(5);
			
		I2C_Write(0x25);					// Envoyer 0x25 code pour demander d'envoyer une données 
		_delay_ms(100);
		
	
		I2C_Repeated_Start(Slave_Read_Address1);	// Repeated Start I2C communication with SLA+R 
		_delay_ms(5);
		
		valFenetre=I2C_Read_Nack(); //lire la donnée (1 seule) et envoyer sur le port D   vérifier l'état de PD0 
		_delay_ms(5);
		
		I2C_Stop();   // Stop I2C
		_delay_ms(5);
		
		//afficher l'état des fenetres
		//fenetre 1
		if ((valFenetre & 0b01000000)==0b00000000) { 
		   PORTC |=(1<<PC1); //allumer temoin fenetre 1
		}
		else {
		   
		   PORTC &=~(1<<PC1); //eteindre Temoin Fenetre 1
		}
		//fenetre 2
		if ((valFenetre & 0b10000000)==0b00000000) {
			PORTC |=(1<<PC2); //allumer temoin fenetre 2
		}
		else {
			
			PORTC &=~(1<<PC2); //eteindre Temoin Fenetre 2
		}
		
		_delay_ms(100);
		
		
		
	// communication avec le Atmega328P Slave 2 Porte I2C -----------------------------------------------------------
		I2C_Start_Wait(Slave_Write_Address2);// Start I2C communication with SLA+W
		_delay_ms(5);
		
		I2C_Write(0x35);					// Envoyer 0x25 code pour demander d'envoyer une données
		_delay_ms(100);
		
		
		I2C_Repeated_Start(Slave_Read_Address2);	// Repeated Start I2C communication with SLA+R
		_delay_ms(5);
		
		valPorte=I2C_Read_Nack(); //lire la donnée (1 seule) et envoyer sur le port D   vérifier l'état de PD0
		_delay_ms(5);
		I2C_Stop();   // Stop I2C
		_delay_ms(5);
		//afficher l'état desporte
		//PORTE
		if ((valPorte & 0b10000000)==0b00000000) {
			PORTC |=(1<<PC0); //allumer temoin PORTE
		}
		else {
			
			PORTC &=~(1<<PC0); //eteindre Temoin Porte
		}
		_delay_ms(100);
			
	//---------------------------------------------------------------------------------------------------------
	
	//communication avec le capteur de temperature i2c ds1621-------------------------------------------------
		// convertir une donnée,
			I2C_Start_Wait(TEMP_W);
			_delay_ms(5);
			I2C_Write(start_conversion);//ordre de conversion
			_delay_ms(5);
			I2C_Stop();
			_delay_ms(5);
	    // lire une temperature 	
			I2C_Start_Wait(TEMP_W);
			_delay_ms(5);
			I2C_Write(read_temperature);//code pour lire la temperature
			_delay_ms(5);
			I2C_Repeated_Start(TEMP_R);
			_delay_ms(5);
			tmp_msb=I2C_Read_Ack();//lecture de la temperature
			_delay_ms(5);
			tmp_lsb=I2C_Read_Nack();//lecture de la partie decimale de la temperature
			_delay_ms(5);
			I2C_Stop();
			_delay_ms(5);
			temperature=tmp_msb; //Température pas de 1oC
			//Temperature=tmp_msb+0.5*(tmp_lsb/128); //Température pas de 0.5oC
		
		//convertir la valeur de la temperature en string et prêt a etre envoyer au ESP
		//temperature=25;
		dtostrf(temperature, 4, 0, aff);
		printString(aff);
		transmitByte(0x0D);
		transmitByte(0x0A);
	//-----------------------------------------------------------------------------------------------------------
	};				
};