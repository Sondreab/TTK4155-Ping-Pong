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

int static BOARD_motor_to_edge(int direction){
	int board_width = 0;
	int count_not_moving = 0;
	int temp = 0;
	BOARD_set_Motor(direction*65);
	_delay_ms(5);
	while(count_not_moving < 5){
		temp = BOARD_get_motor_pos();
		if(temp == 0){
			count_not_moving++;
		}
		
		board_width += (temp/125);
		_delay_ms(1);
	}
	
	count_not_moving = 0;
	BOARD_set_Motor(0);
	BOARD_motor_disable();
	return board_width;
}

int BOARD_initialize_for_game(){
	int board_width = 0;
	
	board_width = BOARD_motor_to_edge(-1);
	
	printf("Right edge\n");
	
	_delay_ms(1000);
	
	board_width = BOARD_motor_to_edge(1);
	
	printf("Left edge\n");
	
	return board_width;
}

void BOARD_set_Motor(int16_t speed){
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
	
	if (speed > 128){
		speed = 128;
	}
	
	uint8_t motor_input = speed;
	
	//set value
	DAQ_setOutput(motor_input);
	
}

void BOARD_motor_disable(){
	PORTH &= ~(1<<PH4);
}

int16_t BOARD_get_motor_pos(){
	//set !OE low to enable output of encoder
	PORTH &= ~(1<<PH5);
	//set SEL low to get high byte
	PORTH &= ~(1<<PH3);
	//wait for stable signal
	_delay_us(20);
	//read high byte on PIND
	uint8_t lowbyte = PINK;
	//set SEL high and wait for stable signal
	PORTH |= (1<<PH3);
	_delay_us(20);
	//read low byte on PIND
	uint8_t highbyte = PINK;
	//toggle !RST to reset the encoder
	PORTH &= ~(1<<PH6);
	_delay_us(10);
	PORTH |= (1<<PH6);
	//set !OE high to disable reading
	PORTH |= (1<<PH5);
	
	uint16_t result = (highbyte<<8);
	result += lowbyte;
	return result;
}

void BOARD_fire_solenoid(){
	PORTF &= ~(1 << PF1);
}

void BOARD_retract_solenoid(){
	PORTF |= (1 << PF1);
}