/*
 * main.c
 *
 * Created: 9/28/2023 1:09:50 PM
 *  Author: 0279137
 *
 *
 */ 
#define F_CPU 1000000UL

#include <xc.h>
#include <util/delay.h>

#define LIGHT PC5

void IO_Init(void)
{
	//SET PC5 as output
	DDRC = 0xFF;
}

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	DDRB = (1<<PB4);
	/* Enable SPI */
	SPCR = (1<<SPE);
}
char SPI_SlaveReceive(void)
{
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	;
	/* Return Data Register */
	return SPDR;
}

int main(void)
{
	uint8_t data;
	IO_Init();
	SPI_SlaveInit();
    while(1)
    {
		data = SPI_SlaveReceive();
		_delay_us(5);
		if(data == 0x01){
			PORTC |= (1<<LIGHT);
		} 
		else{
			PORTC &= ~(1<<LIGHT);
		}
    }
}