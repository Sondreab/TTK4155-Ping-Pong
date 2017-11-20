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
#include "SCORE.h"


#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

volatile char new_unread_message = 0;
volatile char pid_update_flag = 0;
volatile char solenoid_tot_overflow = 0;

volatile char hundred_ms_flag = 0;
volatile char hundred_ms_tot_overflow = 0;

volatile char one_sec_flag = 0;
volatile char one_sec_tot_overflow = 0;

typedef struct board_input_t{
	uint8_t motor_pos;
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
	solenoid_tot_overflow++;
	one_sec_tot_overflow++;
	hundred_ms_tot_overflow++;
	if(solenoid_tot_overflow > 70){
		solenoid_tot_overflow = 0;
		BOARD_retract_solenoid();
	}
	if(one_sec_tot_overflow > 210){
		one_sec_tot_overflow = 0;
		one_sec_flag = 1;		
	}
	if (hundred_ms_tot_overflow > 24){
		hundred_ms_tot_overflow = 0;
		hundred_ms_flag = 1;
	}
}



int detect_ball(){
	int goal_made = 0;
	if(ADC_filtered_read(20) < 10){
		goal_made = 1;
	}
	return goal_made;
}

void format_board_input(struct board_input_t* board_input, struct CAN_msg_t* msg){
	uint8_t motor_pos = 255 - msg->data[0];
	
	board_input->motor_pos = motor_pos;
	board_input->servo_position = msg->data[1];							
	board_input->solenoid_trigger_prev = board_input->solenoid_trigger_curr;
	board_input->solenoid_trigger_curr = msg->data[2];		
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
	
	board_input.motor_pos = 0;
	board_input.servo_position = 0;
	board_input.solenoid_trigger_curr, board_input.solenoid_trigger_prev = 0;
	
	struct PID_DATA pid_data;
	int16_t P_factor = 2; 
	int16_t I_factor = 1;
	int16_t D_factor = 0;
	pid_Init(P_factor*SCALING_FACTOR, I_factor*SCALING_FACTOR, D_factor*SCALING_FACTOR, &pid_data);
	
	
	int16_t encoderData;
	
	int16_t prev_pid_input = 0;
	int16_t pid_input = 0;
	int16_t pid_output = 0;
	
	int16_t current_pos;
	int16_t board_width;
	
	uint16_t score = 0;
	uint16_t highscore = 1337;
	
	int8_t IN_GAME = 0;
	
	// ---- END OF INITIALIZATION ---- //
		
	
	while (1)
	{
		_delay_ms(1);
		if(new_unread_message){
			CAN_data_recieve(&receive_message);
			if(receive_message.id == GAME_START_ID){
				IN_GAME = 1;
				highscore = (receive_message.data[1] << 8);
				highscore += receive_message.data[0];
				printf("Start game message recieved\n");
				
				score = 0;
				board_width = BOARD_initialize_for_game();
				printf("Board width: %i\n", board_width);
				
				SCORE_send_to_node_three(SCORE_START, highscore);
				
				transmit_message.id = GAME_START_ID;
				transmit_message.length = 0;
				
				CAN_message_send(&transmit_message);
				printf("Ack sent\n");
			}
			new_unread_message = 0;
			
		}
		
		while(IN_GAME){
			if(new_unread_message){
				CAN_data_recieve(&receive_message);
				if (receive_message.id == JOY_DATA_ID){
					format_board_input(&board_input, &receive_message);
				}
						
				PWM_set_compare(board_input.servo_position);
				if(board_input.solenoid_trigger_prev == 0 & board_input.solenoid_trigger_curr == 1) {
					BOARD_fire_solenoid();
					solenoid_tot_overflow = 0;
				}
				new_unread_message = 0;
			
			}
			
			current_pos = BOARD_get_motor_pos();
			
			if (one_sec_flag){
// 				printf("Motor: %i\n", board_input.motor_pos)*(board_width/255);
// 				printf("pos: %i\n",current_pos);
// 				printf("in: %i\n", pid_input);
// 				printf("out: %i\n", pid_output);
// 				printf("\n");
				one_sec_flag = 0;
			}
			if (hundred_ms_flag){
				score++;
				SCORE_send_to_node_three(SCORE_UPDATE, score);
				hundred_ms_flag = 0;
			}
			if(pid_update_flag){
				pid_input = board_input.motor_pos*(board_width/255);
				//Update setpoint only if it's > than 5% change
				if (abs(prev_pid_input - pid_input) < (board_width/20)){
					pid_input = prev_pid_input;
				}
				pid_output = pid_Controller(pid_input, current_pos, &pid_data);
				prev_pid_input = pid_input;
				BOARD_set_motor(pid_output);

				pid_update_flag = 0;
			}
			
			if(detect_ball()){
				printf(" --- Game over --- \n\n");
				BOARD_set_motor(0);
				BOARD_motor_disable();
				SCORE_send_to_node_three(SCORE_END, score);
				SCORE_send_to_node_one(score, &transmit_message);
				printf("Game over message sent\n");
				IN_GAME = 0;
			}
		}
	}
 }
