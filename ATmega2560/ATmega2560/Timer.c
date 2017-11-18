/*
 * TIMER_driver.c
 *
 * Created: 02.11.2017 14:09:47
 *  Author: sondreab
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.h"

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
	
	//PWM output on PB6 on atmega -> pin12 on shield
	DDRB |= (1 << PB6);
	
	PWM_set_compare(127); //centre of [0, 255]

}

void timer0_init(){	
	//Prescale cpu clock by 1/256 -> bottom to top = 4.1 ms
	TCCR0B &= ~(1 << CS01) & ~(1 << CS00);
	TCCR0B |= (1 << CS02);
	
	//Enable normal operation Overflow on max = 0xFF
	TCCR0A &= ~(1 << WGM00) & ~(1 << WGM01);
	TCCR0B &= ~(1 << WGM02);
	
	
	//Initialize Timer0
	TCNT0 = 0;
	
	//Overflow interrupt enable
	TIMSK0 = (1 << TOIE0);
}

void PWM_set_compare(int joy_pos){
	
	//Convert joystick position [0, 255] to compare level [2000, 4000] = [1 ms, 2 ms]	
	uint16_t compare_level = 4000 - (float)(joy_pos*(2000.0/255.0)); // 2000/255 = 7,84
	
	
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