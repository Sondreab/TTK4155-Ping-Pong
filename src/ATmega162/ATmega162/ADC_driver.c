/*
 * ADC_driver.c
 *
 * Created: 18.09.2017 13:48:23
 *  Author: hploubat
 */ 

# define F_CPU 4915200UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include "typedef.h"
#include "ADC_driver.h"

volatile static char ADC_intr;



uint8_t ADC_read(uint8_t channel) {
	volatile char *ADC_address = 0x1400;
	*ADC_address = channel +3;
	
	_delay_ms(45);
		
	return *ADC_address;
}