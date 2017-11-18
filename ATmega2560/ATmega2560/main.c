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
#include "Timer.h"
#include "ADC_driver.h"
#include "DAC_driver.h"
#include "BOARD_controller.h"
#include "PID.h"


#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

volatile char new_unread_message = 0;
volatile char pid_update_flag = 0;
volatile char timer0_tot_overflow = 0;

typedef struct board_input_t{
	int8_t motor_speed;
	uint8_t servo_position;
	uint8_t solenoid_trigger_prev;
	uint8_t solenoid_trigger_curr;	
	};

void INTR_init(void){
	
	//CAN interrupt
	DDRD &= ~(1<<PD2);
	
	//Disable global interrupts
	cli();
	
	//Interrupt on falling edge on PD2
	EICRA &= ~(1<<ISC20);
	EICRA |= (1<<ISC21);
	
	//Enable interrupt on PD2
	EIMSK |= (1<<INT2);
	
	//Enable global interrupts
	sei();
}

ISR(INT2_vect){
	new_unread_message = 1;
}

ISR(TIMER0_OVF_vect){
	pid_update_flag = 1;
	timer0_tot_overflow++;
	if(timer0_tot_overflow > 70){
		timer0_tot_overflow = 0;
		BOARD_retract_solenoid();
	}
	
}



int detect_ball(){
	int goal_made = 0;
	if(ADC_filtered_read(10) < 15){
		goal_made = 1;
	}
	return goal_made;
}

void format_board_input(struct board_input_t* board_input, struct CAN_msg_t* msg){
	//CONVERSION ONLY FOR SPEED CONTROL
	float motor_speed = (msg->data[0] - 127)*100;
	motor_speed = -motor_speed/(127);
	if((motor_speed < 15) & (motor_speed > -15)){
		motor_speed = 0;
	}
	
	//CONVERSION FOR POSITION CONTROL
//	uint8_t motor_pos = 255 - msg->data[0];
	
	board_input->motor_speed = motor_speed;
	board_input->servo_position = msg->data[1];							
	board_input->solenoid_trigger_prev = board_input->solenoid_trigger_curr;
	board_input->solenoid_trigger_curr = msg->data[2];		
}


void print_board_input(struct board_input_t* board_input){
	printf("Motor: %i\n", board_input->motor_speed);
	printf("Servo: %i\n", board_input->servo_position);
	printf("Solenoid: %i -> %i\n\n", board_input->solenoid_trigger_prev, board_input->solenoid_trigger_curr);
}


void initialize_node(){
	UART_Init(MYUBRR);
	fdevopen(&UART_Transmit, &UART_Receive);
	DAQ_init();
	INTR_init();
	PWM_init();
	ADC_init();
	BOARD_motor_init();
	timer0_init();
	CAN_init();
	printf(" --- End of initialize ---\n\n");
	
}

int main(void)
{
	initialize_node();

	// ---- VARIABLE AND STRUCT INITIALIZATION ---- //
	volatile struct CAN_msg_t receive_message;
	volatile struct CAN_msg_t transmit_message;
	struct board_input_t board_input;
	
	board_input.motor_speed = 0;
	board_input.servo_position = 0;
	board_input.solenoid_trigger_curr, board_input.solenoid_trigger_prev = 0;
	
	struct PID_DATA pid_data;
	int16_t P_factor = 1;
	int16_t I_factor = 1;
	int16_t D_factor = 0;
	pid_Init(P_factor*SCALING_FACTOR, I_factor*SCALING_FACTOR, D_factor*SCALING_FACTOR, &pid_data);
	
	
	int16_t encoderData;
	
	int16_t pid_output;
	
	
	
	_delay_ms(1000);
		
	
	// ---- LOOP ---- //
	int8_t IN_GAME = 0;
	
	//start game
	while (1)
	{
		_delay_ms(1);
		if(new_unread_message){
			CAN_data_recieve(&receive_message);
			if(receive_message.id == GAME_START_ID){
				IN_GAME = 1;
				printf("Start game message recieved\n");
				
// 				int result = BOARD_initialize_for_game();
// 				printf("board width: %i\n", result);
				
				transmit_message.id = GAME_START_ID;
				transmit_message.length = 0;
				
				CAN_message_send(&transmit_message);
				printf("Ack sent\n");
			}
			new_unread_message = 0;
			
		}
		
		while(IN_GAME){
			_delay_ms(1);
			if(new_unread_message){
				CAN_data_recieve(&receive_message);
				if (receive_message.id == JOY_DATA_ID){
					format_board_input(&board_input, &receive_message);
				}
			
				print_board_input(&board_input);
			
				PWM_set_compare(board_input.servo_position);
				if(board_input.solenoid_trigger_prev == 0 & board_input.solenoid_trigger_curr == 1) {
					BOARD_fire_solenoid();
					timer0_tot_overflow = 0;
				}
				new_unread_message = 0;
			
			}
		
			
			
			if(pid_update_flag){
// 				encoderData = Get_motor_pos();
// 				pid_output = pid_Controller(board_input.motor_speed*INPUT_MATCH, encoderData, &pid_data);
// 				printf("pid output: %i\n\n", pid_output/OUTPUT_MATCH);
				BOARD_set_Motor(board_input.motor_speed);
				pid_update_flag = 0;
			}
			
			if(detect_ball()){
				printf(" --- Game over --- \n\n");
				//shut off motor
				BOARD_motor_disable();
				//send game over message
				transmit_message.id = GAME_OVER_ID;
				transmit_message.length = 1;
				transmit_message.data[0] = 0xFF; //should be score
				CAN_message_send(&transmit_message);
				printf("Game over message sent\n");
				//break game function
				IN_GAME = 0;
			}
		}
	}
 }