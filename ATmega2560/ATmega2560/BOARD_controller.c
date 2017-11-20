/*
 * Motor_controller.c
 *
 * Created: 13.11.2017 14:10:55
 *  Author: hploubat
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "DAC_driver.h"
#include "Timer.h"

void BOARD_motor_init(){
	//set MJ1:EN as output -> atmega:PH4
	DDRH |= 1<<PH4;
	//set MJ1:dir as output -> atmega:PH1
	DDRH |= 1<<PH1;
	
	//set MJ1:!RST as output, then high -> atmega:PH6
	DDRH |= 1<<PH6;
	PORTH |= (1<<PH6);
	//set MJ1:!OE as output -> atmega:PH5
	DDRH |= 1<<PH5;
	//set MJ1:SEL as output -> atmega:PH3
	DDRH |= 1<<PH3;
	//set the output used for the solenoid [A1 on ADCL on Arduino shield]
	DDRF |= (1<<PF1);
	PORTF |= (1<<PF1);
	
// 	BOARD_set_Motor(0);
// 	BOARD_motor_disable();
}

void static BOARD_centre_motor(int board_width){
	int centre = board_width/2;
	int current_pos = board_width;
	BOARD_set_motor(-60);
	_delay_ms(100);
	while (current_pos > centre){
		current_pos = BOARD_get_motor_pos();
		_delay_ms(1);
	}
	printf("Current pos: %i\n", BOARD_get_motor_pos());
	BOARD_set_motor(0);
	BOARD_motor_disable();
}

int static BOARD_motor_to_left_edge(void){
	int board_width = 0;
	int current_encoder = 0;
	int prev_encoder = -1;
	int count_not_moving = 0;
	BOARD_set_motor(70);
	_delay_ms(100);
	while (count_not_moving < 20){
		_delay_ms(5);
		prev_encoder = current_encoder;
		current_encoder = BOARD_get_motor_pos();
		if (prev_encoder == current_encoder){
			count_not_moving++;
		}
	}
	BOARD_set_motor(0);
	BOARD_motor_disable();
	board_width = BOARD_get_motor_pos();
	return board_width;	
}

void static BOARD_motor_to_right_edge(void){
	int current_encoder = 0;
	int prev_encoder = -1;
	int count_not_moving = 0;
	BOARD_set_motor(-70);
	_delay_ms(100);
	while (count_not_moving < 20){
		_delay_ms(5);
		prev_encoder = current_encoder;
		current_encoder = BOARD_get_motor_pos();
		if (prev_encoder == current_encoder){
			count_not_moving++;
		}
	}
	BOARD_set_motor(0);
	BOARD_motor_disable();
}

int BOARD_initialize_for_game(){
	BOARD_enable_encoder();
	int board_width = 0;
	
	BOARD_motor_to_right_edge();
	
	printf("Right edge\n");
	
	BOARD_reset_encoder();
	
	_delay_ms(1000);
	
	board_width = BOARD_motor_to_left_edge();
	
	printf("Left edge\n");
	
	_delay_ms(1000);
	
	BOARD_centre_motor(board_width);
	
	return board_width;
}

void BOARD_set_motor(int16_t speed){
	//enable motor
	PORTH |= (1<<PH4);
	//set direction
	if(speed < 0){
		PORTH |= (1<<PH1);
		speed = -speed;
	} else if(speed > 0){
		PORTH &= ~(1<<PH1);
	} else if(speed == 0){
		PORTH &= ~(1<<PH4);
	}
	
	if (speed > 100){
		speed = 100;
	}
	
	uint8_t motor_input = speed;
	
	//set value
	DAQ_setOutput(motor_input);
	
}

void BOARD_motor_disable(){
	PORTH &= ~(1<<PH4);
}

int16_t BOARD_get_motor_pos(){
	//set SEL low to get high byte
	PORTH &= ~(1<<PH3);
	//wait for stable signal
	_delay_us(20);
	//read high byte on PINK
	uint8_t highbyte = PINK;
	//set SEL high and wait for stable signal
	PORTH |= (1<<PH3);
	_delay_us(20);
	//read low byte on PINK
	uint8_t lowbyte = PINK;

	
	uint16_t result = (highbyte<<8);
	result += lowbyte;
	return result;
}

void BOARD_enable_encoder(){
	//set !OE low to enable output of encoder
	PORTH &= ~(1<<PH5);
}

void BOARD_disable_encoder(){
	//set !OE high to disable reading
	PORTH |= (1<<PH5);
}

void BOARD_reset_encoder(void){
	//toggle !RST to reset the encoder
	 PORTH &= ~(1<<PH6);
	 _delay_us(10);
	 PORTH |= (1<<PH6);
}

void BOARD_fire_solenoid(){
	PORTF &= ~(1 << PF1);
}

void BOARD_retract_solenoid(){
	PORTF |= (1 << PF1);
}