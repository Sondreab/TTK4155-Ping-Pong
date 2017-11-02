/*
 * TIMER_driver.c
 *
 * Created: 02.11.2017 14:09:47
 *  Author: sondreab
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM_driver.h"

void PWM_init(){
	//Set up timer1 with prescaler = 1/8
	TCCR1B &= ~(1 << CS12);
	TCCR1B |= (1 << CS11);
	TCCR1B &= ~(1 << CS10);
	
	//Enable fast pwm
	TCCR1A &= ~(1 << WGM10);
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13); 
	
	//Non-inverted mode on channel B	
	TCCR1A |= (1 << COM1B1);
	TCCR1A &= ~(1 << COM1B0);
	
	//Initialize timer
	TCNT1 = 0;
	
	//Set top to 40,000 -> T = 20 ms
	ICR1H = (40000 >> 8);
	ICR1L = (40000 & 0xff);
	
	DDRB |= (1 << PB6);
	
	
}

void PWM_set_compare(int joy_pos){
	
	//Convert joystick position [-100, 100] to compare level [2000, 4000] = [1 ms, 2 ms]	
	uint16_t compare_level = 3000 + joy_pos*10;
	
	
	//Safety measure to protect hardware
	if (compare_level > 4000){
		compare_level = 4000;
	}
	if(compare_level < 2000){
		compare_level = 2000;
	}
	
	OCR1BH = (compare_level >> 8);
	OCR1BL = (compare_level & 0xff);	
	
}