/*
 * ADC_driver.c
 *
 * Created: 06.11.2017 11:52:16
 *  Author: hploubat
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


void ADC_init(){	
	//Set analog input to ADC0
	ADMUX &= ~(1 << MUX0) & ~(1 << MUX1) & ~(1 << MUX2) & ~(1 << MUX3) & ~(1 << MUX4);
	ADCSRB &= ~(1 << MUX5);
	
	ADCSRA |= (1<< ADPS2) | (1 << ADPS1) | (1 << ADPS2);
	
	//set AVCC as reference
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	
	
	//Set PF0 as input
	PORTF &= ~(1 << PF0);
	
	//Set Left-adjusted result bits because we only need 8 bit resolution -> descrepency of 4 lsb = +- 20 mV
	ADMUX &= ~(1 << ADLAR);
	
	//Enable ADC
	ADCSRA |= (1 << ADEN);
	
}

uint16_t ADC_read(){
	ADCSRA |= (1 << ADSC);

	//wait for conversion to finish
	while(ADCSRA & (1 << ADSC));
	
	uint16_t result = (ADCL) | (ADCH << 8);
	ADCSRA &= ~(1 << ADIF);
	return result;
}

uint16_t ADC_filtered_read(int numReadings){
	int result = 0;
	for(int readN = 0; readN < numReadings; readN++){
		result = result + ADC_read();
	}
	uint16_t filtered_result = result/numReadings;
	return filtered_result;
}


void ADC_sleep(){
	
}