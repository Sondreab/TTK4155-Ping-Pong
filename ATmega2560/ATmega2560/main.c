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

void print_message_details(struct CAN_msg_t* msg){
	printf("Received ID: %i\n", msg->id);
	
	printf("\tX: %i\n", msg->data[0]);
	printf("\tY: %i\n", msg->data[1]);
	switch (msg->data[3]){
		case 0:
		printf("\tDir: NEUTRAL\n");
		break;
		
		case 1:
		printf("\tDir: UP\n");
		break;
		
		case 2:
		printf("\tDir: RIGHT\n");
		break;
		
		case 3:
		printf("\tDir: DOWN\n");
		break;
		
		case 4:
		printf("\tDir: LEFT\n");
		break;
		
		default:
		break;
	}
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
	struct PID_DATA pid_data;
	int16_t P_factor = 1;
	int16_t I_factor = 1;
	int16_t D_factor = 0;
	pid_Init(P_factor*SCALING_FACTOR, I_factor*SCALING_FACTOR, D_factor*SCALING_FACTOR, &pid_data);
	
	int totGoals = 0;
	
	int16_t encoderData;
	
	int16_t pid_output;
	
	
	// ---- LOOP ---- //
	
	_delay_ms(1000);
	
	while (1)
	{
		_delay_us(1);
		if(new_unread_message){
			CAN_data_recieve(&message_received);
			new_unread_message = 0;
	
			print_message_details(&message_received);
			
			PWM_set_compare(message_received.data[0]);
			if((message_received.data[5]>>2)) {
				Fire_solenoid();
			}
			
		}
		
		if(detect_goal()){
			totGoals++;
			printf("\n----  Goal detected!  ----\n\n");
			printf("----  Total goals: %i  ----\n", totGoals);
			_delay_ms(1000);
		}
		
		if(pid_update_flag){
			encoderData = Get_motor_pos();
			pid_output = pid_Controller(1*INPUT_MATCH, encoderData, &pid_data);
			printf("pid output: %i\n", pid_output/OUTPUT_MATCH);
			Set_Motor(pid_output/LEVEL_MATCH);
			pid_update_flag = 0;
		}
		
	}
}