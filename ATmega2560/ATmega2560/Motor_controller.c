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

void Motor_init(){
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
	//set the output used for the solenoid
	DDRF |= (1<<PF1);
	PORTF |= (1<<PF1);
}

void Set_Motor(int16_t speed){
	//enable motor
	PORTH |= (1<<PH4);
	//set direction
	if(speed < 0){
		PORTH &= ~(1<<PH1);
	} else if(speed > 0){
		PORTH |= (1<<PH1);
	} else {
		PORTH &= ~(1<<PH4);
	}
	
	speed = abs(speed);
	if (speed > 128){
		speed = 128;
	}
	
	//set value
	DAQ_setOutput((int8_t)speed);
	
	_delay_ms(100);
	PORTH &= ~(1<<PH4);
}

int16_t Get_motor_pos(){
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

void Fire_solenoid(){
	PORTF &= ~(1<<PF1);
	_delay_ms(100);
	PORTF |= (1<<PF1);
}