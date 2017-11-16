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
#define GAME_OVER_ID 0x01
#define GAME_START_ID 0x02
#define JOY_DATA_ID 0x0F

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


void joystick_message_packager(struct JOY_data_t* joy_state, struct CAN_msg_t* msg)
{
	msg->id = JOY_DATA_ID;
	msg->length = 6;
	
	uint8_t buttons = (joy_state->joy_button << 2) | (joy_state->L_button << 1) | (joy_state->R_button);
	
	msg->data[0] = joy_state->position.X;
	msg->data[1] = joy_state->position.Y;
	msg->data[2] = joy_state->direction;
	msg->data[3] = joy_state->sliders.L_slider;
	msg->data[4] = joy_state->sliders.R_slider;
	msg->data[5] = buttons;
}

int Play_game(struct CAN_msg_t *receive_msg, struct CAN_msg_t *transmit_msg){
	int score = 0;
// 	int minutes;
// 	int seconds;
	//Implement messages sent from node 2 with score updates?
	printf("Waiting for game-start-ack from node 2\n");
	while (new_unread_message==0); //waiting for confirm from node 2
	new_unread_message = 0;
	CAN_data_recieve(&receive_msg);
	if (receive_msg->id != GAME_START_ID){
		return -1; //Something went wrong
	}
	
	printf("Ack received\n\n");
	
	volatile struct JOY_data_t previous_joy_state;
	JOY_initialize_state(&previous_joy_state);
	volatile struct JOY_data_t current_joy_state;
	JOY_initialize_state(&current_joy_state);
	
	while (1){
		_delay_us(1);
		
		
		if(JOY_poll_change(&previous_joy_state, &current_joy_state)){
			joystick_message_packager(&current_joy_state, &transmit_msg);
			CAN_message_send(&transmit_msg);
		}
		
		if (new_unread_message == 1){
			CAN_data_recieve(&receive_msg);
			new_unread_message = 0;
		}
		
		if (receive_msg->id == GAME_OVER_ID){
			score = receive_msg->data[0];
			printf("Received game over message\n\n");
// 			minutes = receive_msg->data[1];
// 			seconds = receive_msg->data[2];
			break;
		}
		
		
				
	}
	
	return score;	
}


void initialize_node(){
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	INTR_init();
	XMEM_init();
	XMEM_test();
	JOY_init();
	OLED_init();
	OLED_reset();
	CAN_init();
	printf(" --- End of initialization --- \n\n");
}


int main(void){
	initialize_node();
	
	volatile struct CAN_msg_t transmit_msg;
	volatile struct CAN_msg_t receive_msg;
	
	while (1)
	{
		transmit_msg.id = GAME_START_ID;
		transmit_msg.length = 0;
		
		CAN_message_send(&transmit_msg);
		
		printf("Start game message sent \n");		
		
		
		Play_game(&receive_msg, &transmit_msg);
		
		printf("Game ended, Click LEFT touch button to play again\n\n");
		
		while(1){
			if(JOY_button(1)){
				printf("Starting game again.\n\n");
				break;
			}
		}
	}
	
}

