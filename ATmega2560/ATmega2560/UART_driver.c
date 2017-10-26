

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include "UART_driver.h"


#define FOSC 1843200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1


void UART_Init( unsigned int ubrr )
{
	//Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}


void UART_Transmit(char data)
{
	/* Wait for empty transmit buffer*/
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data*/
	UDR0 = data;
	//return 0;
}

int UART_Receive(void){
	//waiting for data to be received
	while( !(UCSR0A & (1<<RXC0)) );
	return UDR0;
}

void UART_Test(void) {
	char recievedMessage = UART_Receive();
	UART_Transmit(recievedMessage);
}

