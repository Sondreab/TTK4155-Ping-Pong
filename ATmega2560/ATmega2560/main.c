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
#include "Motor_controller.h"
#include "PID.h"


#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

volatile char new_unread_message = 0;
volatile char pid_update_flag = 0;

typedef struct board_input_t{
	int8_t motor_speed;
	int8_t servo_position;
	int8_t solenoid_trigger_prev;
	int8_t solenoid_trigger_curr;	
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
}

int detect_goal(){
	int goal_made = 0;
	if(ADC_filtered_read(10) < 25){
		goal_made = 1;
	}
	return goal_made;
}

void extract_message_data(struct board_input_t* board_input, struct CAN_msg_t* msg){
	board_input->motor_speed = msg->data[0];
	board_input->servo_position = msg->data[1];							//NOT FINALIZED
	board_input->solenoid_trigger_prev = board_input->solenoid_trigger_curr;
	board_input->solenoid_trigger_curr = (msg->data[5] >> 2);		//NOT FINALIZED
}

void print_message_details(struct CAN_msg_t* msg){
	printf("Received ID: %i\n", msg->id);
	
	printf("\tX: %i\n", msg->data[0]);
	printf("\tY: %i\n", msg->data[1]);
// 	switch (msg->data[3]){
// 		case 0:
// 		printf("\tDir: NEUTRAL\n");
// 		break;
// 		
// 		case 1:
// 		printf("\tDir: UP\n");
// 		break;
// 		
// 		case 2:
// 		printf("\tDir: RIGHT\n");
// 		break;
// 		
// 		case 3:
// 		printf("\tDir: DOWN\n");
// 		break;
// 		
// 		case 4:
// 		printf("\tDir: LEFT\n");
// 		break;
// 		
// 		default:
// 		break;
// 	}
	int joyb = msg->data[5] >> 2;
	int Lb = ((msg->data[5] & 0b010) >> 1);
	int Rb = (msg->data[5] & 0b001);
	printf("\tLeft Slider: %i\n", (uint8_t)msg->data[3]);
	printf("\tRight Slider: %i\n", (uint8_t)msg->data[4]);
	printf("\tButtons: \n\t\tJoy: %i \n\t\tLeft: %i \n\t\tRight: %i\n\n", joyb, Lb, Rb);
}

int main(void)
{
	UART_Init(MYUBRR);
	fdevopen(&UART_Transmit, &UART_Receive);
	DAQ_init();
	INTR_init();
	CAN_init();
	PWM_init();
	ADC_init();
	Motor_init();
	PID_timer_init();
	printf("End of init\n");

	// ---- VARIABLE AND STRUCT INITIALIZATION ---- //
	volatile struct CAN_msg_t message_received;
	struct board_input_t board_input;
	
	board_input.motor_speed = 0;
	board_input.servo_position = 0;
	board_input.solenoid_trigger_curr, board_input.solenoid_trigger_prev = 0;
	
	struct PID_DATA pid_data;
	int16_t P_factor = 1;
	int16_t I_factor = 1;
	int16_t D_factor = 0;
	pid_Init(P_factor*SCALING_FACTOR, I_factor*SCALING_FACTOR, D_factor*SCALING_FACTOR, &pid_data);
	
	int totGoals = 0;
	
	int16_t encoderData;
	
	int16_t pid_output;
	
	
	// ---- LOOP ---- //
	
	
	
	while (1)
	{
		_delay_ms(1);
		if(new_unread_message){
			CAN_data_recieve(&message_received);
			if (message_received.id == 0x0f){
				extract_message_data(&board_input, &message_received);
			}
			
			printf("Motor: %i\n", board_input.motor_speed);
			printf("Servo: %i\n", board_input.servo_position);
			printf("Solenoid: %i -> %i\n\n", board_input.solenoid_trigger_prev, board_input.solenoid_trigger_curr);
			
			//print_message_details(&message_received);
			
			PWM_set_compare(board_input.servo_position);
			if(board_input.solenoid_trigger_prev == 0 & board_input.solenoid_trigger_curr == 1) {
				Fire_solenoid();
			}
			new_unread_message = 0;
			
		}
		
		if(detect_goal()){
			totGoals++;
			printf("\n----  Goal detected!  ----\n\n");
			printf("----  Total goals: %i  ----\n", totGoals);
			_delay_ms(1000);
		}
		
		if(pid_update_flag){
// 			encoderData = Get_motor_pos();
// 			pid_output = pid_Controller(board_input.motor_speed*INPUT_MATCH, encoderData, &pid_data);
// 			printf("pid output: %i\n\n", pid_output/OUTPUT_MATCH);
			Set_Motor(board_input.motor_speed/1.5);
			pid_update_flag = 0;
		}
		
	}
}