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
#include <avr/interrupt.h>
#include "UART_driver.h"
#include "XMEM_driver.h"
#include "JOY_driver.h"
#include "ADC_driver.h"
#include "OLED_driver.h"
#include "MENU.h"
#include "CAN_driver.h"


#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

char new_unread_message;

void INTR_init(void){
	
	//CAN interrupt
	DDRE &= ~(1<<PD3);
	
	//ADC interrupt
	DDRE &= ~(1<<PE0);
	
	//Disable global interrupts
	cli();
	
	//Interrupt on falling edge on PD3 and PE0
	MCUCR &= ~(1<<ISC10);
	MCUCR |= (1<<ISC11);
	EMCUCR |= (1<<ISC2);
	
	//Enable interrupt on PD3 and PE0
	GICR |= (1<<INT1);
	GICR |= (1<<INT2);
	
	//Enable global interrupts
	sei();
}

	// -- ISR FOR INT2 IS IMPLEMENTED IN ADC_driver.c FOR DEPENDENCY REASONS -- //

ISR(INT1_vect){
	new_unread_message = 1;
}


int main(void){
	
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	INTR_init();
	XMEM_init();
	XMEM_test();
	OLED_init();
	OLED_reset();
	CAN_init();
	
	volatile struct CAN_msg_t message_received;
	
	struct CAN_msg_t msg;
	msg.id = 2047;
	msg.length = 2;
	msg.data[0] = 7;
	msg.data[1] = 80;
	printf("Before send: %i %i\n", msg.data[0], msg.data[1]);
	CAN_message_send(&msg);
	
	_delay_ms(1000);
	
	
	while (new_unread_message != 1);
	CAN_data_recieve(&message_received);
	new_unread_message = 0;
	
	printf("Received ID: %i\n", message_received.id);
	printf("Received data: %i %i\n", message_received.data[0], message_received.data[1]);

}

