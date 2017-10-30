/*
 * ATmega2560.c
 *
 * Created: 26.10.2017 14:58:32
 *  Author: hploubat
 */ 


#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "UART_driver.h"

#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)
int main(void)
{
	UART_Init(MYUBRR);
	char data = "a";
	DDRB = 0xFF;
	while (1)
	{
		PORTB |= (1 << PB7);
		UART_Transmit(0xC3);
		_delay_ms(500);
		PORTB &= ~(1 << PB7);
		
	}
	
}