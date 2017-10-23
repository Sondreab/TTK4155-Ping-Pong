/*
 * ADC_driver.c
 *
 * Created: 18.09.2017 13:48:23
 *  Author: hploubat
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include "ADC_driver.h"

static char ADC_intr;

ISR(INT2_vect){
	ADC_intr = 1;
}

uint8_t ADC_read(uint8_t channel) {
	volatile char *ADC_address = 0x1400;
	*ADC_address = channel +3;
	
	while(ADC_intr != 1);
	ADC_intr = 0;
		
	return *ADC_address;
}