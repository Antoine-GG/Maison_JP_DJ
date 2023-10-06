// Authors:  Idir Chahed, JP Toutant, Jeremy Diaphrase Lachappelle, ChatGPT
// Date: 2023-9-20

#define F_CPU 1000000UL

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>							/* Include standard I/O header file */
#include <string.h>
#include "I2C_Master_H_file.h"				/* Include I2C header file */
#include "houseStatus.h"
#include "i2c.h"
#include "i2c.cpp"

#define SLAVE1        0x3C ///0011 1100
#define SLAVE2        0x5A ///0101 1010
#define SLAVE3        2 //
#define SLAVE4        3 //
#define SLAVE5        4 //
#define useI2C 	      0 //
#define useII2C       1 //
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
	UBRR0H = 0;              /* baud rate  */
	UBRR0L = 0x0C;           /* 9600 */
	UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
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
		transmitByte(myString[i]);
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
	 uint8_t tmp_msb = i2cReadAck(); //lecture MSB
	 uint8_t tmp_lsb = i2cReadNoAck(); //lecture LSB
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
int main() {
	
	double Temperature;
	int8_t tmp_msb,tmp_lsb;
	uint8_t received;
	if(useII2C)initIi2c();
	if(useSPI) SPI_MasterInit();
	initUSART();
	if(useI2C) I2C_Init();
	if(useManualI2C) initManualI2c();
	_delay_ms(500); // Attendre pour laisser les ATmega328P esclaves s'initialiser
	
	while (1) {

		received = 0;
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
		SPI_MasterTransmit(status.light, SLAVE4);
		//POLL U5, get the keyboard char
		status.keyboardChar = SPI_MasterTransmit('Y', SLAVE5);
		}
		if(useII2C){
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
		 
		 //SLAVE1
		 i2cStart();
		 i2cSend(SLAVE1);
		 i2cSend(0x25);
		 i2cStart(); // start
		 i2cSend(SLAVE1 | 0b00000001);
		 tmp_msb = i2cReadAck(); //lecture MSB
		 tmp_lsb = i2cReadNoAck(); //lecture LSB
		 i2cStop();
		 _delay_ms(5);
		 //status.window1 =
		 	   
		}
		//uart 
		status.window1
		firstByte = 
		uartTransmitByte();
		
		_delay_ms(5); // Attendre avant de refaire la demande
	}

	return 0;
}