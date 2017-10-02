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
#include "JOY_driver.h"
#include "ADC_driver.h"
#include "OLED_driver.h"


#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1


int main(void){
	
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	
	INTR_init();
	XMEM_init();
	XMEM_test();
	OLED_init();
	OLED_reset();
	//JOY_init();
	
	OLED_pos(0,0);
	
	char array[12] = "Hello World";
	
	OLED_print(array, sizeof(array));
	//size_t size = sizeof(array) / sizeof(array[0]);
//
	//for(int i = 0; i <size-1; i++){
		//printf("%c", array[i]);
	//}
	
}

