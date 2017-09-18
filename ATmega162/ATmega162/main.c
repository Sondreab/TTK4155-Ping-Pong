/*
 * main.c
 *
 * Created: 11.09.2017 11:06:58
 *  Author: hploubat
 */ 



# define F_CPU 4915200UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "UART_driver.h"
#include "XMEM_driver.h"
#include "ADC_driver.h"

#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

int main(void){
	
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	
	
	XMEM_init();
	
	
	XMEM_test();
	
	//while (1) {
		//_delay_ms(1000);
		//UART test
		
		//printf("Hello World!");
		//recievedMessage = UART_Receive();
		//UART_Transmit(recievedMessage);
	//}
		
		
		
		//((uint8_t *)0x181F)[0] =1;
	//}

	while(1){
		uint8_t Xcoord = ADC_read(1);
		printf("%02x", Xcoord);
		_delay_ms(1000);
	}
}