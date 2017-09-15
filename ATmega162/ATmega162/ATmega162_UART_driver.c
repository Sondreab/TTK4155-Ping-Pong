/*
 * ATmega162.c
 *
 * Created: 04.09.2017 12:17:36
 *  Author: sondreab
 */ 

#define F_CPU 4915200
#define BAUD 9600

#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include "ATmega162_UART_driver.h"


//UTIL functions (temporary placement)
/*
#define set_bit( reg, bit ) (reg |= (1 << bit))
#define clear_bit( reg, bit ) (reg &= ~(1 << bit))
#define test_bit( reg, bit ) (reg & (1 << bit))
#define loop_until_bit_is_set( reg, bit ) while( !test_bit( reg, bit ) )
#define loop_until_bit_is_clear( reg, bit ) while( test_bit( reg, bit ) )
*/

void uart_init(void) {
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

char uart_getchar(void) {
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}







//testing what happens when we send something from ATmega162
/*
int main(void) {
	//char received = uart_getchar();
	//uart_putchar(received);
	char input = "m";
	while(1) {
		uart_putchar(input);
		_delay_ms(100);
	}
	
}
*/

