/*
 * ADC_driver.c
 *
 * Created: 18.09.2017 13:48:23
 *  Author: hploubat
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>

uint8_t ADC_read(uint8_t channel) {
	//configure channel/mode
	//need to be in adress space of ADC and have single ended mode
	volatile char * ADC_address = 0x1400;
	*ADC_address = channel +3;
	
	_delay_us(45);
	
	return ADC_address;
	
	
	
	//select mode by writing to the data buss
	//set /WR low
	
	
	//read the ADC result after /INTR goes low. set /RD low to start reading.
}