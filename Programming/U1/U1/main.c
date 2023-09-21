#include <xc.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#define F_CPU 1000000UL
#include <util/delay.h>


#define SLAVE_ADDRESS 0x03//addresse i2c de U1

char buffer[8];
volatile uint8_t window1 = 0;
volatile uint8_t window2 = 0;
volatile uint8_t variableValue = 0;
uint8_t windows =0;

void initI2Cslave(void) {
	//mode slave
	TWCR = 0x04;
	TWAR = (SLAVE_ADDRESS << 1); //set l'adresse slave
	TWBR = 2;                               /* set bit rate, */
	/* 8MHz / (16+2*TWBR*1) ~= 100kHz */
	TWCR |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN); // Enable I2C, enable ACK, enable interrupt;
	sei();  // Enable global interrupts
}
void i2c_send(unsigned char data){
	TWDR = data ;
	
}

void i2cWaitForComplete(void) {
	loop_until_bit_is_set(TWCR, TWINT);
}

void i2cStart(void) {
	TWCR = (1 <<TWINT) | (1 <<TWEN) | (1 <<TWSTA);
	i2cWaitForComplete();
}

void i2cStop(void) {
	TWCR = (1 <<TWINT) | (1 <<TWEN) | (1 <<TWSTO);
}

uint8_t i2cReadAck(void) {
	TWCR =(1 <<TWINT) | (1 <<TWEN) | (1 <<TWEA);
	i2cWaitForComplete();
	return (TWDR);
}

uint8_t i2cReadNoAck(void) {
	TWCR = (1 <<TWINT) | (1 <<TWEN);
	i2cWaitForComplete();
	return (TWDR);
}

void i2cSend(uint8_t data) {
	TWDR = data;
	TWCR = (1 <<TWINT) | (1 <<TWEN);                  /* init and enable */
	i2cWaitForComplete();
}


//Were using UART for debug
void initUSART(void) {
	UBRR0H = 0;              /* baud rate  */
	UBRR0L = 0x0C;           /* 9600 */
	UCSR0A |= (1 << U2X0);      /* mode asynchrone double vitesse */
	UCSR0B |= (1 << TXEN0); //| (1 << RXEN0);    /* Activer emission et reception  USART */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit, valeur au reset*/
}
//transmettre un caractère
void transmitByte(uint8_t data) {
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Attendre que le buffer de transmission soit vide */
	UDR0 = data;                      /* envoyer la donnée */
}
//recevoir un caractère
uint8_t receiveByte(void) {
	while ( !( UCSR0A & (1<<RXC0)) ); /* Attendre que le buffer de réception soit plein */
	return UDR0;                                /* retourner la valeur lue */
}
//envoyer une chaine de caractères
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}
void uint8ToBinaryString(uint8_t value, char* buffer) {
	for (int i = 7; i >= 0; i--) {
		buffer[7 - i] = ((value >> i) & 0x01) ? '1' : '0';
	}
	buffer[8] = '\0';
}

void TWI_Slave_Stop(void) {
	TWCR &= ~((1 << TWEN) | (1 << TWIE));  // Disable TWI and TWI Interrupt
}

ISR(TWI_vect) {
	uint8_t status = TWSR & 0xF8;
	transmitByte('A');
	PORTB |= (1 << PB0);
	switch (status) {
		case TW_SR_SLA_ACK:  // Address received, ACK returned
		// Handle address received event (optional)
		break;
		case TW_SR_DATA_ACK:  // Data received, ACK returned
		// Handle data received event
		  // Store received value in a variable
		break;
		case TW_ST_SLA_ACK:  // Address transmitted, ACK received
		case TW_ST_DATA_ACK:  // Data transmitted, ACK received
		// Respond to read requests (optional)
		TWDR = variableValue;  // Send the variable value to the master
		break;
		case TW_SR_STOP:  // Stop condition received while addressed
		// Handle stop condition received event (optional)
		break;
		default:
		 TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
	}

	// Clear TWI interrupt flag, enable TWI
	TWCR |= (1 << TWINT);         // Clear interrupt flag
	//TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

void initIOports(){

	// Set PC4 and PC5 as output pin for debug
	DDRC |= (1 << PC3);
	DDRC |= (1 << PC2);
	DDRC |= (1 << PC1);
	DDRB |= (1 << PB0);

	// Set PD6 and PD7 as input pins
	DDRD &= ~(1 << PD6);
	DDRD &= ~(1 << PD7);
	

	// Enable pull-up resistors on PD3 and PD4
	//PORTD |= (1 << 3);
	//PORTD |= (1 << 4);
}

int main(void) {
	initI2Cslave();
	initIOports();
	initUSART();

	while (1) {
		//windows =0;
		// Your main code logic here
		window1 = PIND & (1 << PD6);
		window2 = PIND & (1 << PD7);
		windows |= (window1 << 0);
		windows |= (window2 << 1);
		
		_delay_ms(100);
		transmitByte('A');
		//transmitByte(windows);
		//uint8ToBinaryString(windows, buffer);
		//printString(buffer);
		//if(window2 == 0){
			//PORTB |= (1 << PB0);
			//transmitByte(0x61);
		//}
		//else
		//{
			//transmitByte(2);
			//PORTB &= ~(1 << PB0);
		//}
		
	}

	return 0;
}