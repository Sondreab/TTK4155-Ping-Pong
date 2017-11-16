/*
 * CAN_driver.c
 *
 * Created: 09.10.2017 13:35:56
 *  Author: marmad
 */ 

# define F_CPU 4915200UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "SPI_driver.h"
#include "MCP2515_driver.h"
#include "MCP2515.h"
#include "CAN_driver.h"


uint8_t CAN_init() {
	SPI_MasterInit();
	mcp2515_reset();
	_delay_ms(1);
	char value = mcp2515_read(MCP_CANSTAT);
	//self-test
	if ((value & MODE_MASK) != MODE_CONFIG) {
		printf("MCP2515 is not in config mode after reset! val: %x\n",value);
		return 1;
	}
	
	_delay_ms(1);
	mcp2515_bit_modify(MCP_RXB0CTRL, MCP_RXB0CTRL_SET_FILTERS_OFF);
	_delay_ms(1);
	mcp2515_bit_modify(MCP_RXB0CTRL, MCP_RXB0CTRL_SET_ROLLOVER_OFF);
	_delay_ms(1);
	mcp2515_bit_modify(MCP_CANINTE, MCP_CANINTE_RXB0_ENABLE);
	_delay_ms(1);
	
	mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL_SET_MODE_NORMAL);
	
	return 0;
}

void CAN_message_send(struct CAN_msg_t* msg){
	char check;
	char intr;
	do{
		check = mcp2515_read(MCP_TXB0CTRL);
	}while ((check & MCP_TXB0CTRL_TEST_TX_REQUEST));
	
	mcp2515_write(MCP_TXB0_SIDH, msg->id >> 3);
	mcp2515_write(MCP_TXB0_SIDL, msg->id << 5);
	mcp2515_write(MCP_TXB0_DLC, msg->length);
	for (int i = 0; i < msg->length; i++){
		mcp2515_write(MCP_TXB0_D0 + i, msg->data[i]);
	}
	for(int i = msg->length; i < 8; i++){
		mcp2515_write(MCP_TXB0_D0 + i, 0);
	}
	
	mcp2515_request_to_send(MCP_RTS_TX0);
}

void CAN_data_recieve(struct CAN_msg_t* msg){
	char IDh = mcp2515_read(MCP_RXB0_SIDH);
	char IDl = mcp2515_read(MCP_RXB0_SIDL);
	char length = mcp2515_read(MCP_RXB0_DLC);
	
	msg->id = (IDh << 3 | IDl >> 5);
	msg->length = (length & 0x0f);
	for (int i = 0; i < msg->length; i++){
		msg->data[i] = mcp2515_read(MCP_RXB0_D0 + i);
	}
	for(int i = msg->length; i < 8; i++){
		msg->data[i] = 0;
	}
	mcp2515_bit_modify(MCP_CANINTF, MCP_CANINTF_CLEAR_RXB0);
	
}

void CAN_transmit_complete();
void CAN_int_vect();
void CAN_error();


//NOTES:
//the MAB register will overwrite everything in the receive buffer when a new message is received that passes the acceptance criteria, 
//the MCU must clear the CANINTF.RXnIF bit to signify that it's "done" with a message.
//Rollover? 4.2.1
