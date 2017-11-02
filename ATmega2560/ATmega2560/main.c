/*
 * ATmega2560.c
 *
 * Created: 26.10.2017 14:58:32
 *  Author: hploubat
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "UART_driver.h"
#include "SPI_driver.h"
#include "MCP2515.h"
#include "MCP2515_driver.h"
#include "CAN_driver.h"
#include "PWM_driver.h"


#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

volatile char new_unread_message = 0;

void INTR_init(void){
	
	//CAN interrupt
	DDRD &= ~(1<<PD0);
	
	//Disable global interrupts
	cli();
	
	//Interrupt on falling edge on PD0
	EICRA &= ~(1<<ISC00);
	EICRA |= (1<<ISC01);
	
	//Enable interrupt on PD0
	EIMSK |= (1<<INT0);
	
	//Enable global interrupts
	sei();
}

ISR(INT0_vect){
	new_unread_message = 1;
}


int main(void)
{
	UART_Init(MYUBRR);
	fdevopen(&UART_Transmit, &UART_Receive);
	INTR_init();
	CAN_init();
	PWM_init();

	volatile struct CAN_msg_t message_received;
	
	while (1)
	{
		_delay_us(1);
		if(new_unread_message){
			CAN_data_recieve(&message_received);
			new_unread_message = 0;
	
			printf("Received ID: %i\n", message_received.id);
			for(int data_byte = 0; data_byte < message_received.length; data_byte++){
				printf("\tReceived data[%i]: %i\n", data_byte, message_received.data[data_byte]);
			}
			
			PWM_set_compare(message_received.data[0]);
			
		}
		
	}
	
	
}