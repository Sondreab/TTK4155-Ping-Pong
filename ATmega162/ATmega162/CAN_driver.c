/*
 * CAN_driver.c
 *
 * Created: 09.10.2017 13:35:56
 *  Author: marmad
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "SPI_driver.h"
#include "MCP2515_driver.h"
#include "MCP2515.h"
#include "CAN_driver.h"

uint8_t CAN_init() {
	uint8_t value;
	SPI_MasterInit();
	mcp2515_reset();
	_delay_ms(100);
	//mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL_SET_MODE_CONFIG);
	//self-test
	mcp2515_read(MCP_CANSTAT, &value);
	if ((value & MODE_MASK) != MODE_CONFIG) {
		printf("MCP2515 is not in config mode after reset! val: %x\n",value);
		return 1;
	}
	
	mcp2515_bit_modify(MCP_CANINTE, MCP_CANINTE_RXB0_1_ENABLE); //additional interrupts?
	mcp2515_bit_modify(MCP_RXB0CTRL, MCP_RXB0CTRL_SET_FILTERS_OFF);
	mcp2515_bit_modify(MCP_RXB0CTRL, MCP_RXB0CTRL_SET_ROLLOVER_OFF);
	
	mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL_SET_MODE_LOOPBACK);
	
	return 0;
}

void CAN_message_send(struct CAN_msg_t* msg){
	char check;
	do{
		mcp2515_read(MCP_TXB0CTRL, &check);
	}while ((check & MCP_TXB0CTRL_TEST_TX_REQUEST));
	
	mcp2515_write(msg->id >> 3, MCP_TXB0_SIDH);
	mcp2515_write(msg->id << 5, MCP_TXB0_SIDL);
	mcp2515_write(msg->length, MCP_TXB0_DLC);
	for (int i = 0; i < msg->length; i++){
		mcp2515_write(msg->data[i], MCP_TXB0_D0 + i);
	}
	mcp2515_request_to_send(MCP_RTS_TXB0);
	
}

struct CAN_msg_t CAN_data_recieve(){
	
	struct CAN_msg_t msg;
	
	memset(&msg, 0, sizeof(struct CAN_msg_t));
	char IDh;
	char IDl;
	char intr;
	char length;
	mcp2515_read(MCP_RXB0_SIDH, &IDh);
	mcp2515_read(MCP_RXB0_SIDL, &IDl);
	mcp2515_read(MCP_CANINTF, &intr);
	mcp2515_read(MCP_RXB0_DLC, &length);
	
	printf("intr:%x\n", intr);
	
	if(intr & MCP_CANINTF_TEST_RXB0_FULL){
		printf("RXB0 full");
		msg.id = (IDh << 3 | IDl >> 5);
		msg.length = (length & 0x0f);
		for (int i = 0; i < msg.length; i++){
			mcp2515_read(MCP_RXB0_D0 + i, msg.data[i]);
		}
		for(int i = msg.length; i < 8; i++){
			msg.data[i] = 0;
		}
		mcp2515_bit_modify(MCP_CANINTF, MCP_CANINTF_CLEAR_RXB0);
	//}elseif(mcp2515_read(MCP_CANINTF) & MCP_CANINTF_TEST_RXB1_FULL){
		//msg.id = (mcp2515_read(MCP_RXB1_SIDH) << 3 | mcp2515_read(MCP_RXB1_SIDL) >> 5); //Need addr of TXB1_SIDL.
		//msg.length = (mcp2515_read(MCP_RXB1_DLC) & 0x0f);
		//for (int i = 0; i < msg.length; i++){
			//msg.data[i] = mcp2515_read(MCP_RXB0_D0 + i);
		//}
		//for(int i = msg.length; i < 8; i++){
			//msg.data[i] = 0;
		//}
	}
	
	return msg;	
}

void CAN_transmit_complete();
void CAN_int_vect();
void CAN_error();


//NOTES:
//the MAB register will overwrite everything in the receive buffer when a new message is received that passes the acceptance criteria, 
//the MCU must clear the CANINTF.RXnIF bit to signify that it's "done" with a message.
//Rollover? 4.2.1
