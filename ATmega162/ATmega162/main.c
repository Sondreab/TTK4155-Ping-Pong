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

ISR(BADISR_vect) {
	printf("PANIC!\n");
}


void joystick_message_packager(struct JOY_data_t* joy_state, struct CAN_msg_t* msg)
{
	int settings = 1;
	msg->id = JOY_DATA_ID;
	msg->length = 3;
	switch(settings){
		//User defined settings cases
		default:
			
		case 1:
		msg->data[0] = joy_state->sliders.R_slider;
		msg->data[1] = joy_state->position.X;
		msg->data[2] = joy_state->R_button;
		break;
	}
	
}

int Play_game(struct CAN_msg_t* transmit_msg, struct CAN_msg_t* receive_msg, struct JOY_data_t* current_joy_state, struct JOY_data_t* previous_joy_state){
		int score = 0;
		int change = 0;
		while (1){
			
			_delay_ms(1000);
			
			transmit_msg->id = GAME_START_ID;
			transmit_msg->length = 0;
			
			CAN_message_send(transmit_msg);
			
			int score = 0;
			printf("W f ack\n");
			
			while (new_unread_message==0){
				_delay_ms(20);
			}
			
			CAN_data_recieve(receive_msg);
			new_unread_message = 0;
			if (receive_msg->id == GAME_START_ID){
				printf("Ack rec\n");
			}
			
			
			while (1){
				_delay_ms(1);
				
				change = JOY_poll_change(previous_joy_state, current_joy_state);
				if(change){
					joystick_message_packager(current_joy_state, transmit_msg);
					CAN_message_send(transmit_msg);
				}
				if (new_unread_message == 1){
					CAN_data_recieve(receive_msg);
					new_unread_message = 0;
					if (receive_msg->id == GAME_OVER_ID){
						score = receive_msg->data[0];
						return score;
					}
				}
			}
		}
}


const char init_complete_str[] = "-- End of init --\n";
const char game_over_str[] = "         Game Over!       ";
const char score_str[] = "         Score: ";

int main(void){
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	INTR_init();
 	XMEM_init();
 	XMEM_test();
	OLED_init();
	OLED_reset();
	JOY_init();
	CAN_init();
	printf(init_complete_str);
	
	
	menu_t *mainMenu = MENU_init();
	
 	struct CAN_msg_t transmit_msg;
 	struct CAN_msg_t receive_msg;
	
	struct JOY_data_t previous_joy_state;
	JOY_initialize_state(&previous_joy_state);
	
	struct JOY_data_t current_joy_state;
	JOY_initialize_state(&current_joy_state);

	STATE_t STATE = MENU;
	int score = 743;
	
	
	//------ TESTING OF MENU ------------
	
	
	_delay_ms(500);
	// -------- END TESTING OF MENU --------
	while (1){
		
		
		
		
		switch(STATE){
				
			case MENU:
				STATE = MENU_controller(mainMenu);
				break;
			case PLAY_GAME:
				score = Play_game(&transmit_msg, &receive_msg, &current_joy_state, &previous_joy_state);
				STATE = GAME_OVER;
				break;
			case SET_BRIGHTNESS:
				MENU_set_brightness();
				STATE = MENU;
				break;
			case GAME_OVER:
				OLED_reset();
				OLED_pos(1,0);
				NEW_OLED_print(game_over_str);
				char score_chars[3];
				itoa(score, score_chars, 10);
				OLED_pos(2,0);
				NEW_OLED_print(score_str);
				NEW_OLED_print(score_chars);
				_delay_ms(3000);
				STATE = MENU;
				break;
			default:
				break;
		}
	}
}

