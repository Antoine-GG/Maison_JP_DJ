//JP Toutant
//sounds without the clock PWMs
//23 sept 2023
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

void playTone(char note){
	switch (note)
	{
		case 'a':
		for(int i = 0; i <= 1000; i += 2)
		{
			PORTB |= (1 << PB1);
			_delay_us(110);
			PORTB &= ~(1 << PB1);
			_delay_us(110);
		}
		PORTB &= ~(1 << PB1);
		break;
		case 'b':
		for(int i = 0; i <= 1000; i += 16)
		{
			PORTB |= (1 << PB1);
			_delay_us(1024);
			PORTB &= ~(1 << PB1);
			_delay_us(1024);
		}
		PORTB &= ~(1 << PB1);
		break;
		case 'c':
		for(int i = 0; i <= 1000; i += 8)
		{
			PORTB |= (1 << PB1);
			_delay_us(512);
			PORTB &= ~(1 << PB1);
			_delay_us(512);
		}
		PORTB &= ~(1 << PB1);
		break;
		case 'd':
		for(int i = 0; i <= 1000; i += 12)
		{
			PORTB |= (1 << PB1);
			_delay_us(440);
			PORTB &= ~(1 << PB1);
			_delay_us(440);
		}
		PORTB &= ~(1 << PB1);
		break;
		case 'e':
		for(int i = 0; i <= 1000; i += 10)
		{
			PORTB |= (1 << PB1);
			_delay_us(2200);
			PORTB &= ~(1 << PB1);
			_delay_us(2200);
		}
		PORTB &= ~(1 << PB1);
		break;
		case 'f':
		for(int i = 0; i <= 1000; i += 8)
		{
			PORTB |= (1 << PB1);
			_delay_us(1360);
			PORTB &= ~(1 << PB1);
			_delay_us(1360);
		}
		PORTB &= ~(1 << PB1);
		break;
		case 'g':
		for(int i = 0; i <= 1000; i += 220)
		{
			PORTB |= (1 << PB1);
			_delay_us(1360);
			PORTB &= ~(1 << PB1);
			_delay_us(1360);
		}
		PORTB &= ~(1 << PB1);
		break;
	}
	
	
}
void sound_success(){
	playTone('b');
	playTone('c');
	playTone('a');
}
void sound_input(){
	playTone('d');
}
void sound_fail(){
	playTone('f');
	playTone('e');
}
void sound_inputLost(){
	playTone('g');
}