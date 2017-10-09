/*
 * CAN_driver.c
 *
 * Created: 09.10.2017 13:35:56
 *  Author: marmad
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "SPI_driver.h"
#include "MCP2515_driver.h"
#include "MCP2515.h"

uint8_t CAN_init() {
	uint8_t value;
	SPI_MasterInit();
	mcp2515_reset();
	
	//self-test
	mcp2515_read(MCP_CANSTAT, &value);
	if ((value & MODE_MASK) != MODE_CONFIG) {
		printf("MCP2515 is not in config mode after reset!\n");
		return 1;
	}
	
	
	
	//TODO: the rest of init
	
	//11 or 29 bit identifier? Register 3-2
	
	
	
	return 0;
}

//NOTES:
//the MAB register will overwrite everything in the recieve buffer when a new message is received that passes the acceptance criteria, 
//the MCU must clear the CANINTF.RXnIF bit to signify that it's "done" with a message.
//Rollover? 4.2.1
