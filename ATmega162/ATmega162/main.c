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
#include "typedef.h"
#include "UART_driver.h"
#include "XMEM_driver.h"
#include "JOY_driver.h"
#include "ADC_driver.h"
#include "OLED_driver.h"
#include "MENU.h"
#include "MCP2515.h"
#include "CAN_driver.h"
#include "USER_profiles.h"


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


void joystick_message_packager(struct JOY_data_t* joy_state, struct CAN_msg_t* msg, USER_controls_t* controls){
	msg->id = JOY_DATA_ID;
	msg->length = 3;
	switch(controls->motor_pos){
		case L_SLIDER:
			msg->data[MOTOR_POS] = joy_state->sliders.L_slider;
			break;
		case X_JOY:
			msg->data[MOTOR_POS] = joy_state->position.X;
			break;
		case Y_JOY:
			msg->data[MOTOR_POS] = joy_state->position.Y;
			break;
		case R_SLIDER:
		default:
			msg->data[MOTOR_POS] = joy_state->sliders.R_slider;
			break;
	}
	
	switch(controls->servo){
		case L_SLIDER:
			msg->data[SERVO] = joy_state->sliders.L_slider;
			break;
		case R_SLIDER:
			msg->data[SERVO] = joy_state->sliders.R_slider;
			break;
		case Y_JOY:
			msg->data[SERVO] = joy_state->position.Y;
			break;
		case X_JOY:
		default:
			msg->data[SERVO] = joy_state->position.X;
			break;	
	}
	
	switch(controls->solenoid){
		case JOY_BUTTON:
			msg->data[SOLENOID] = joy_state->joy_button;
			break;
		case L_BUTTON:
			msg->data[SOLENOID] = joy_state->L_button;
			break;
		case R_BUTTON:
		default:
			msg->data[SOLENOID] = joy_state->R_button;
			break;
		
	}
	
// 	msg->data[0] = joy_state->sliders.R_slider;
// 	msg->data[1] = joy_state->position.X;
// 	msg->data[2] = joy_state->R_button;
	
}

char game_board_init_str[] = "Initializing game board";
char please_wait_str[] = "     Please wait...";
char game_running[] = "  --- Game running! ---";

uint16_t Play_game(struct CAN_msg_t* transmit_msg, struct CAN_msg_t* receive_msg, struct JOY_data_t* current_joy_state, struct JOY_data_t* previous_joy_state, USER_profile_t* user, uint16_t highscore){
	uint16_t score = 0;
	int hw_change = 0;
	
	OLED_reset();
	OLED_pos(3,0);
	NEW_OLED_print(game_board_init_str);
	OLED_pos(4,0);
	NEW_OLED_print(please_wait_str);
	
	_delay_ms(500);
			
	transmit_msg->id = GAME_START_ID;
	transmit_msg->length = 2;
	transmit_msg->data[0] = (highscore & 0b11111111);
	transmit_msg->data[1] = (highscore >> 8);
			
	CAN_message_send(transmit_msg);
			
	score = 0;
	printf("W f ack\n");
			
	while (new_unread_message==0){
		_delay_ms(20);
	}
			
	CAN_data_recieve(receive_msg);
	new_unread_message = 0;
	if (receive_msg->id == GAME_START_ID){
		printf("Ack rec\n");
	}
	
	OLED_reset();
	OLED_pos(3,0);
	NEW_OLED_print(game_running);
			
	while (1){
		_delay_ms(1);
				
		hw_change = JOY_poll_change(previous_joy_state, current_joy_state);
		if(hw_change){
			joystick_message_packager(current_joy_state, transmit_msg, &user->ctrl_pref);
			CAN_message_send(transmit_msg);
		}
		if (new_unread_message == 1){
			CAN_data_recieve(receive_msg);
			new_unread_message = 0;
			if (receive_msg->id == GAME_OVER_ID){
				score = (receive_msg->data[1] << 8);
				score += receive_msg->data[0];
				return score;
			}
		}
	}
}


char init_complete_str[] = "-- End of init --\n";
char game_over_str[] = "       Game Over!";
char score_str[] = "       Score: ";
char continue_promt_str[] = "    R button for menu";

void Game_over(uint16_t score){
	OLED_reset();
	OLED_pos(1,0);
	NEW_OLED_print(game_over_str);
	char score_chars[5];
	itoa(score, score_chars, 10);
	OLED_pos(2,0);
	NEW_OLED_print(score_str);
	NEW_OLED_print(score_chars);
	OLED_pos(4,0);
	NEW_OLED_print(continue_promt_str);
	while (1){
		if (JOY_button(R_BUTTON)){
			break;
		}
	}
	_delay_ms(500);
}


void Set_controls_motor_pos(USER_controls_t* controls){
	OLED_reset();
	NEW_OLED_print("Motor position controls:");
	OLED_pos(1,20);
	NEW_OLED_print("Press button:");
	OLED_pos(3,20);
	NEW_OLED_print("R: R-slider (default)");
	OLED_pos(4,20);
	NEW_OLED_print("L: L-slider");
	OLED_pos(5,20);
	NEW_OLED_print("Joy: X-axis");
	while (1){
		if(JOY_button(R_BUTTON)){
			controls->motor_pos = R_SLIDER;
			break;
		}
		if (JOY_button(L_BUTTON)){
			controls->motor_pos = L_SLIDER;
			break;
		}
		if (JOY_button(JOY_BUTTON)){
			controls->motor_pos = X_JOY;
			break;
		}		
	}
	_delay_ms(500);
}

void Set_controls_servo(USER_controls_t* controls){
	OLED_reset();
	NEW_OLED_print("Servo controls:");
	OLED_pos(1,20);
	NEW_OLED_print("Press button:");
	OLED_pos(3,20);
	NEW_OLED_print("Joy: X-axis (default)");
	OLED_pos(4,20);
	NEW_OLED_print("L: L-slider");
	OLED_pos(5,20);
	NEW_OLED_print("R: R-slider");
	while (1){
		if (JOY_button(JOY_BUTTON)){
			controls->servo = X_JOY;
			break;
		}
		if (JOY_button(L_BUTTON)){
			controls->servo = L_SLIDER;
			break;
		}
		if(JOY_button(R_BUTTON)){
			controls->servo = R_SLIDER;
			break;
		}
	}
	_delay_ms(500);
}

void Set_controls_solenoid(USER_controls_t* controls){
	OLED_reset();
	NEW_OLED_print("Solenoid controls:");
	OLED_pos(1,20);
	NEW_OLED_print("Press button:");
	OLED_pos(3,20);
	NEW_OLED_print("R: R-button (default)");
	OLED_pos(4,20);
	NEW_OLED_print("L: L-button");
	OLED_pos(5,20);
	NEW_OLED_print("Joy: Joy-button");
	while (1){
		if(JOY_button(R_BUTTON)){
			controls->solenoid = R_BUTTON;
			break;
		}
		if (JOY_button(L_BUTTON)){
			controls->solenoid = L_BUTTON;
			break;
		}
		if (JOY_button(JOY_BUTTON)){
			controls->solenoid = JOY_BUTTON;
			break;
		}
	}
	_delay_ms(500);
}

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
	
	menu_t* mainMenu = 0x1801; //tyring to push main menu to external RAM, because malloc() makes data memory overflow.
	mainMenu = MENU_init();
	
	USER_profile_t default_profile;
	default_profile.name = "Default";
	default_profile.highscore = 1337;
	default_profile.ctrl_pref.motor_pos = R_SLIDER;
	default_profile.ctrl_pref.servo = X_JOY;
	default_profile.ctrl_pref.solenoid = R_BUTTON;
	
 	struct CAN_msg_t transmit_msg;
 	struct CAN_msg_t receive_msg;
	
	struct JOY_data_t previous_joy_state;
	JOY_initialize_state(&previous_joy_state);
	
	struct JOY_data_t current_joy_state;
	JOY_initialize_state(&current_joy_state);

	STATE_t STATE = MENU;
	uint16_t highscore = 1337;
	uint16_t score = 0;
	
	USER_profile_t * active_user = &default_profile;
	
	//------ TESTING OF MENU ------------
	
	
	_delay_ms(500);
	// -------- END TESTING OF MENU --------
	while (1){
		switch(STATE){
			case MENU:
				STATE = MENU_controller(mainMenu);
				break;
			case SETTINGS:
				STATE = MENU_controller(mainMenu->submenus[1]);
				break;
			case PLAY_GAME:
				score = Play_game(&transmit_msg, &receive_msg, &current_joy_state, &previous_joy_state, active_user, highscore);
				if (score > highscore){
					highscore = score;
				}
				STATE = GAME_OVER;
				break;
			case SET_BRIGHTNESS:
				MENU_set_brightness();
				STATE = MENU;
				break;
			case CAL_JOY:
				JOY_calibrate();
				STATE = MENU;
				break;
			case GAME_OVER:
				Game_over(score);
				STATE = MENU;
				break;
			case SET_MOTOR_POS:
				Set_controls_motor_pos(&active_user->ctrl_pref);
				STATE = SETTINGS;
				break;
			case SET_SERVO:
				Set_controls_servo(&active_user->ctrl_pref);
				STATE = SETTINGS;
				break;
			case SET_SOLENOID:
				Set_controls_solenoid(&active_user->ctrl_pref);
				STATE = SETTINGS;
				break;
			default:
				break;
		}
	}
}

