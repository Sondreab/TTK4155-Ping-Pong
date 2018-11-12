/*
 * DAC_driver.c
 *
 * Created: 13.11.2017 12:01:12
 *  Author: hploubat
 */ 

#include <avr/io.h>
#include "TWI_Master.h"

void DAQ_init(){
	TWI_Master_Initialise();
	DDRD |= (1<<PD0);
	DDRD |= (1<<PD1);
}

void DAQ_setOutput(char value){
	unsigned char* TWI_message = malloc(3*sizeof(char));
	
	TWI_message[0] = 0x50;
	TWI_message[1] = 0x00;
	TWI_message[2] = value;
	
	TWI_Start_Transceiver_With_Data(TWI_message, 0x03);
	free(TWI_message);
}