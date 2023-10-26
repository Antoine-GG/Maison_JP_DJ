// Authors:  Idir Chahed, JP Toutant, Jeremy Diaphrase Lachappelle, ChatGPT
// Date: 2023-9-20

#define F_CPU 8000000UL

#include <xc.h>
#include <avr/io.h>
//#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>							/* Include standard I/O header file */
#include <string.h>
#include <avr/sfr_defs.h>
#include "I2C_Master_H_file.h"				/* Include I2C header file */
#include "houseStatus.h"
#include "i2c.h"
#include "i2c.cpp"

#define Slave_Write_Address1     0x3C     //Atemga 328P Slave Ecriture Slave 1 Fenetres
#define Slave_Read_Address1		 0x3D    //Atemga 328P Slave Lecture  Slave 1  fenetres
#define Slave_Write_Address2	 0x5A     //Atemga 328P Slave Ecriture Slave 2 Porte
#define Slave_Read_Address2		 0x5B
#define SLAVE1	      0x3C ///0011 1100
#define SLAVE2        0x5A ///0101 1010
#define SLAVE3        2 //
#define SLAVE4        3 //
#define SLAVE5        4 //
#define useI2C 	      0 //
#define useII2C       0 //
#define useIdir2C	  0 //
#define useManualI2C  0 // i2c avec la violence
#define useSPI        1 //
#define SDApin		  4 // 
#define SCLpin		  5 //

#define TEMP_W    0b10010000   //0x90  adresse du capteur de température en écriture
#define TEMP_R    0b10010001   //0x91  addresse du capteur de température en lecture
#define read_temperature 0xAA
#define access_config 0xAC //control register
#define start_conversion 0xEE

#define temp_h 0xA1 //temperature high byte
#define temp_l 0xA2 //temperature low byte

uint8_t firstByte();
uint8_t secondByte();
uint8_t thirdByte();

uint8_t tmp_msb;
uint8_t tmp_lsb;


HouseStatus status;

//SPI
void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output SS,  */
	DDRB = (1<<PB3)|(1<<PB5)|(1<<PB2);
	// set Port C pins for 3 ss	
	DDRD = (1<<SLAVE3)|(1<<SLAVE4)|(1<<SLAVE5);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
char SPI_MasterTransmit(char cData, uint8_t ss)
{
	//set all SS high
	PORTD |= (1<<SLAVE3)|(1<<SLAVE4)|(1<<SLAVE5);
	/* Set le slave qu'on veut low */
	PORTD &= ~(1<<ss);
	_delay_ms(1);
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

//init UART
void initUSART(void) {
	UBRR0H = UBRRH_VALUE;              /* baud rate  */
	UBRR0L = UBRRL_VALUE;           /* 9600 */
	//UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);    /* Activer emission et reception  USART */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit, valeur au reset*/
}
//transmettre un caractere
void uartTransmitByte(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Attendre que le buffer de transmission soit vide */
	UDR0 = data;                      /* envoyer la donn�e */
}
//recevoir un caractere
uint8_t uartReceiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); /* Attendre que le buffer de r�ception soit plein */
	return UDR0;                                /* retourner la valeur lue */
}
//envoyer une chaine de caracteres
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		uartTransmitByte(myString[i]);
		i++;
	}
}
//i2c
uint8_t poll_I2cDevice(char address, char codeWord){
		uint8_t data;
		I2C_Start_Wait(address); // wait for ACK signal
		_delay_ms(5);

		I2C_Write(codeWord); //numero de l'instruction attendu par slave
		_delay_ms(500);

		I2C_Repeated_Start(address | 0b00000001); // force Least Meaningful bit to 1 for read mode
		_delay_ms(5);

		data = I2C_Read_Ack();
		_delay_ms(500);
		
		I2C_Stop();
		return data;
}

//manual i2c
void initManualI2c(void){
	// Set PC4 and PC5 as output pins
	DDRC |= (1 << SDApin) | (1 << SCLpin);
	PORTC |= (1 << SDApin) | (1 << SCLpin);
	_delay_ms(1);
}
void startCondition(){
	//SDA goes low before SCL
	_delay_us(5);
	PORTC &= ~(1 << SDApin);
	_delay_us(5);
	PORTC &= ~(1 << SCLpin);
	_delay_us(10);
}
void sendByte(char byte){
	uint8_t reversedByte;
	_delay_us(10);
	//first I need to reverse this byte
	for (int i = 0; i < 8; i++) {
		if (byte & (1 << i)) {
			reversedByte |= (1 << (7 - i));
		}
	}
	// Output each bit of dataByte on PC4
	for (int i = 0; i < 10; i++){
		// Check the current bit and set PC4 accordingly
		if (reversedByte & (1 << i)){
			PORTC |= (1 << SDApin);  // Set PC4 high
		}
		else
		{
			PORTC &= ~(1 << SDApin); // Set PC4 low
		}

		// Create a clock pulse by toggling PC5
		PORTC |= (1 << SCLpin);
		_delay_us(10); // Adjust delay as needed for your clock frequency
		PORTC &= ~(1 << SCLpin);
		_delay_us(10); // Adjust delay as needed for your clock
	}
}
uint8_t receiveByte(){
    // Set SDA pin as input
    DDRD &= ~(1 << SDApin);
    
    uint8_t data = 0;
    
    // Read each bit, MSB first
    for (uint8_t i = 0; i < 8; i++) {
	    // Set SCL pin high
	    PORTD |= (1 << SCLpin);
	    _delay_us(10);
	    
	    // Read SDA pin and store the bit
	    if (PIND & (1 << SDApin))
	    data |= (1 << (7 - i));
	    
	    // Set SCL pin low
	    PORTD &= ~(1 << SCLpin);
	    _delay_us(10);
    }
    
    return data;
    }
void stopCondition() {
	// Set SDA and SCL pins as output
	DDRD |= (1 << SDApin) | (1 << SCLpin);
	
	// Generate stop condition
	PORTD &= ~(1 << SDApin);
	_delay_us(4);
	PORTD |= (1 << SCLpin);
	_delay_us(4);
	PORTD |= (1 << SDApin);
	_delay_us(4);
}
//ii2c
void initIi2c(){

	initI2C();
	// DS1621 initialization,début
	i2cStart();
	i2cSend(TEMP_W);
	i2cSend(access_config);
	i2cSend(0x03); // mode , POL=1 , 1 Shot = 1
	i2cStop();
	_delay_ms(5);
	// Setup de la température TH
	i2cStart();
	i2cSend(TEMP_W);
	i2cSend(0xA1);
	i2cSend(23);
	i2cSend(0b10000000);
	//1 ou 0	 pour la decimal
	i2cStop();
	//Setup de la température TL
	i2cStart();
	i2cSend(TEMP_W);
	i2cSend(0xA2);
	i2cSend(21);
	i2cSend(0b00000000);
}
uint8_t poll_ii2cDevice(uint8_t address, uint8_t command){
	 i2cStart();
	 i2cSend(address);
	 i2cSend(command);
	 i2cStart(); // start
	 i2cSend(address | 0b00000001);
	 tmp_msb = i2cReadAck(); //lecture MSB
	 tmp_lsb = i2cReadNoAck(); //lecture LSB
	 i2cStop();
	 _delay_ms(5);
	 //Temperature=tmp_msb+0.5*(tmp_lsb/128);
	 return tmp_lsb;
}

uint8_t sendByte_ii2c(uint8_t address, uint8_t data){
	 i2cStart();
	 i2cSend(address);
	 i2cSend(data);
	 i2cStop();
	 _delay_ms(5);
}
//Idir2c
init1621(){
	
	I2C_Start_Wait(TEMP_W);  //Start I2C communication with SLA+W
	_delay_ms(5);
	I2C_Write(access_config);
	_delay_ms(5);
	I2C_Write(0x03);    // execution en mode ordre de conversion
	_delay_ms(5);
	I2C_Stop();
	_delay_ms(5);	
}
char poll_Slave1(){
	
	char valFenetre;
	// communication avec le Atmega328P Slave 1 Fenetres I2C -----------------------------------------------------------
	I2C_Start_Wait(Slave_Write_Address1);// Start I2C communication with SLA+W
	_delay_ms(5);
	
	I2C_Write(0x25);					// Envoyer 0x25 code pour demander d'envoyer une donn�es
	_delay_ms(100);
	
	
	I2C_Repeated_Start(Slave_Read_Address1);	// Repeated Start I2C communication with SLA+R
	_delay_ms(5);
	
	valFenetre=I2C_Read_Nack(); //lire la donn�e (1 seule) et envoyer sur le port D   v�rifier l'�tat de PD0
	_delay_ms(5);
	
	I2C_Stop();   // Stop I2C
	_delay_ms(5);
	return valFenetre;
}
char poll_Slave2(){
	char valPorte;
	// communication avec le Atmega328P Slave 2 Porte I2C -----------------------------------------------------------
	I2C_Start_Wait(Slave_Write_Address2);// Start I2C communication with SLA+W
	_delay_ms(5);
	
	I2C_Write(0x35);					// Envoyer 0x25 code pour demander d'envoyer une donn�es
	_delay_ms(100);
	
	
	I2C_Repeated_Start(Slave_Read_Address2);	// Repeated Start I2C communication with SLA+R
	_delay_ms(5);
	
	valPorte=I2C_Read_Nack(); //lire la donn�e (1 seule) et envoyer sur le port D   v�rifier l'�tat de PD0
	_delay_ms(5);
	I2C_Stop();   // Stop I2C
	_delay_ms(5);
	return valPorte;
	//afficher l'�tat desporte
	//PORTE
	if ((valPorte & 0b10000000)==0b00000000) {
		PORTC |=(1<<PC0); //allumer temoin PORTE
	}
	else {
		
		PORTC &=~(1<<PC0); //eteindre Temoin Porte
	}
	_delay_ms(100);
}
double poll_DS1621(){
	
	double temperature;
	//communication avec le capteur de temperature i2c ds1621-------------------------------------------------
	// convertir une donn�e,
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
	temperature=tmp_msb; //Temp�rature pas de 1oC
	//Temperature=tmp_msb+0.5*(tmp_lsb/128); //Temp�rature pas de 0.5oC
	
	//convertir la valeur de la temperature en string et pr�t a etre envoyer au ESP
	//temperature=25;
	return temperature;
	//------------------------
}

int main() {
	
	double Temperature;
	int8_t tmp_msb,tmp_lsb;
	uint8_t received;
	if(useIdir2C){
		I2C_Init();
		init1621();		
	}
	if(useII2C)initIi2c();
	if(useSPI) SPI_MasterInit();
	initUSART();
	if(useI2C) I2C_Init();
	if(useManualI2C) initManualI2c();
	_delay_ms(500); // Attendre pour laisser les ATmega328P esclaves s'initialiser
	
	while (1) {

		received = 0;
		if(useIdir2C){
			char windows= poll_Slave1();
			if ((windows & 0b01000000)==0b00000000) {
					status.window1 = 1; 
			}
			else {
					
					status.window1 = 0;
			}
				

			if ((windows & 0b10000000)==0b00000000) {
					status.window2 = 1; 
			}
			else {
					
				status.window2 = 0; 
			}
				
			_delay_ms(100);
			char valdoor = poll_Slave2();
			if ((valdoor & 0b10000000)==0b00000000) {
				status.door = 1; //allumer temoin PORTE
			}
			else {
				
				status.door = 0; //eteindre Temoin Porte
			}
			_delay_ms(100);
			
			
		}
		
		if(useI2C){
		//i2c calls
		//POLL U1, window1 and window2 are the 2 last bits as 0000 0012
		received = poll_I2cDevice(SLAVE1, 0x25);
		status.window2 = received & 0x01; //Mask the lsb
		status.window1 = (received >> 1) & 0x01; // Shift right by 1 and mask with 0x01
		//POLL U2, get the door status;
		received = poll_I2cDevice(SLAVE2, 0x25);
		status.door = received & 0x01; //Mask the lsb
		
		}
		if(useManualI2C){
			startCondition();
			sendByte(SLAVE1);
			sendByte(0x25);
			received = receiveByte();
			stopCondition();
			status.window2 = received & 0x01; //Mask the lsb
			status.window1 = (received >> 1) & 0x01; // Shift right by 1 and mask with 0x01
		}
		if(useSPI){
		//spi calls
		//Set U3 door lock status
		//SPI_MasterTransmit(status.lock, SLAVE3);
		//Set U4 light status
		//manque la source ou ramener light
		SPI_MasterTransmit(status.light, SLAVE4);
		//POLL U5, get the keyboard char
		status.keyboardChar = SPI_MasterTransmit('Y', SLAVE5);
		uartTransmitByte(status.keyboardChar);

		}
		//if(useII2C){
		if(useIdir2C){ //useIdir2C qui est utilise
			//DS1621 
		 // convertir une donnée,
		 i2cStart();
		 i2cSend(TEMP_W);
		 i2cSend(start_conversion);
		 i2cStop();
		 _delay_ms(5);
		 
		 //demarrer la lecture
		 i2cStart();
		 i2cSend(TEMP_W);
		 i2cSend(read_temperature);
		 i2cStart(); // start
		 i2cSend(TEMP_R);
		 tmp_msb = i2cReadAck(); //lecture MSB
		 tmp_lsb = i2cReadNoAck(); //lecture LSB
		 i2cStop();
		 _delay_ms(5);
		 Temperature=tmp_msb+0.5*(tmp_lsb/128);	
		 
		 //SLAVE1   a verifier
		 /* i2cStart();
		 i2cSend(SLAVE1);
		 i2cSend(0x25);
		 i2cStart(); // start
		 i2cSend(SLAVE1 | 0b00000001);
		 tmp_msb = i2cReadAck(); //lecture MSB
		 tmp_lsb = i2cReadNoAck(); //lecture LSB
		 i2cStop();
		 _delay_ms(5);
		 //status.window1 =
		 	   */
		}
		//uart 
		//firstByte = status.keyboardChar;
		//manque la source ou ramener keyboard doit etre definit
		uartTransmitByte(status.keyboardChar);
		
		_delay_ms(5); // Attendre avant de refaire la demande
	}

	return 0;
}