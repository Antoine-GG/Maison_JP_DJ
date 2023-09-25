/*
 * main.c
 *
 * Created: 9/25/2023 1:28:40 PM
 *  Author: 0279137
 */ 

#define F_CPU 1000000UL
#include <avr/io.h> 
#include <xc.h>
#include <util/delay.h>

void initIO(void){
	// Set PC4 and PC5 as output pins
	DDRC |= (1 << PC4) | (1 << PC5);
	PORTC |= (1 << PC4) | (1 << PC5);
	_delay_ms(1);
}

void startCondition(){
	//SDA goes low before SCL
	_delay_us(5);
	PORTC &= ~(1 << PC4);
	_delay_us(5);
	PORTC &= ~(1 << PC5);
	_delay_us(10);
}

//uint8_t mode, 
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
			PORTC |= (1 << PC4);  // Set PC4 high
			} 
			else
			{
			PORTC &= ~(1 << PC4); // Set PC4 low
			}

		// Create a clock pulse by toggling PC5
		PORTC |= (1 << PC5);
		_delay_ms(10); // Adjust delay as needed for your clock frequency
		PORTC &= ~(1 << PC5);
		_delay_ms(10); // Adjust delay as needed for your clock
		}
}

int main(void)
{
	initIO();
	startCondition();
	_delay_us(30);
	sendByte(0xA0);
	while(1){
		
	}
	return 0;
}