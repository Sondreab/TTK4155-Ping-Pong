/*
 * ADC_driver.c
 *
 * Created: 18.09.2017 13:48:23
 *  Author: hploubat
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "ADC_driver.h"

static volatile uint8_t ADC_intr = 0;

void INTR_init(void){
	
	//ADC interrupt
	DDRE &= ~(1<<PE0);
	
	//Disable global interrupts
	cli();
	
	//Interrupt falling edge PE0
	EMCUCR &= ~(1<<ISC2);
	
	//Enable interrupt on PE0
	GICR |= (1<<INT2);
	
	//Enable global interrupts
	sei();
}

ISR(INT2_vect){
	ADC_intr = 1;
}

uint8_t ADC_read(uint8_t channel) {
	//configure channel/mode
	//need to be in adress space of ADC and have single ended mode
	volatile char *ADC_address = 0x1400;
	*ADC_address = channel +3;
	
	
	while(!ADC_intr);
	ADC_intr = 0;
	//_delay_us(45);
	
	return *ADC_address;
	
	
	//select mode by writing to the data buss
	//set /WR low
	
	
	//read the ADC result after /INTR goes low. set /RD low to start reading.
}