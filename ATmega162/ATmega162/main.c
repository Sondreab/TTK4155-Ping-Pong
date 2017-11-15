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
#include "MCP2515.h"
#include "CAN_driver.h"


#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

volatile char new_unread_message;

void INTR_init(void){
	
	//CAN interrupt
	DDRE &= ~(1<<PD3);
	
	//Disable global interrupts
	cli();
	
	//Interrupt on falling edge on PD3
	MCUCR &= ~(1<<ISC10);
	MCUCR |= (1<<ISC11);
	
	//Enable interrupt on PD3
	GICR |= (1<<INT1);
	
	//Enable global interrupts
	sei();
}

ISR(INT1_vect){
	new_unread_message = 1;
}


void joystick_message_packager(struct CAN_msg_t* msg)
{
	msg->id = 0x0f;
	msg->length = 6;
	struct JOY_position_t position = JOY_getPosition();
	enum JOY_direction_t direction = JOY_getDirection();
	struct JOY_sliders_t sliders = JOY_getSliderPosition();
	char buttons = 0b000;
	for (int button = 0; button < NUM_BUTTONS; button++){
		buttons |= (JOY_button(button) << (NUM_BUTTONS-(button+1)));
	}
	msg->data[0] = position.X;
	msg->data[1] = position.Y;
	msg->data[2] = direction;
	msg->data[3] = sliders.L_slider;
	msg->data[4] = sliders.R_slider;
	msg->data[5] = buttons;	
	_delay_ms(1);
}


int main(void){
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	INTR_init();
	XMEM_init();
	XMEM_test();
	JOY_init();
	OLED_init();
	OLED_reset();
	CAN_init();
	
	volatile struct CAN_msg_t transmit_msg;
	volatile struct CAN_msg_t receive_msg;
	
// 	struct JOY_position_t position;
// 	enum JOY_direction_t direction; 
// 	struct JOY_sliders_t sliders;
	
	while (1) //While game is running
	{
		joystick_message_packager(&transmit_msg);
		CAN_message_send(&transmit_msg);
		
		_delay_ms(25);
	}
	
	
	

}

