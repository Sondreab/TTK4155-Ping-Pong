/*
 * SCORE.c
 *
 * Created: 20.11.2017 12:55:17
 *  Author: marmad
 */ 

#include <avr/io.h>
#include "SCORE.h"
#include "CAN_driver.h"
#include "UART_driver.h"


void SCORE_send_to_node_three(uint8_t id, uint16_t score){

	uint8_t score_lowbyte = (score & 0b11111111);
	uint8_t score_highbyte = (score >> 8);
	
	UART_3_Transmit(id);
	UART_3_Transmit(score_lowbyte);
	UART_3_Transmit(score_highbyte);
}

 void SCORE_send_to_node_one(uint16_t score, struct CAN_msg_t* transmit_message){
 	
 	uint8_t score_lowbyte = (score & 0b11111111);
	uint8_t score_highbyte = (score >> 8);
 	
 	transmit_message->id = GAME_OVER_ID;
 	transmit_message->length = 2;
 	transmit_message->data[0] = score_lowbyte;
 	transmit_message->data[1] = score_highbyte;
 	CAN_message_send(transmit_message);
 }