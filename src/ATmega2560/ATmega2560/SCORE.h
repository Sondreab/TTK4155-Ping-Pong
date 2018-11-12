/*
 * SCORE.h
 *
 * Created: 20.11.2017 12:55:43
 *  Author: marmad
 */ 

typedef enum{
	SCORE_START = 0,
	SCORE_UPDATE = 0x01,
	SCORE_END = 0xFF,
	}SCORE_msg_id_t;

void SCORE_send_to_node_three(SCORE_msg_id_t id, uint16_t score);
/*void SCORE_send_to_node_one(uint16_t score, struct CAN_msg_t* transmit_message);*/

#ifndef SCORE_H_
#define SCORE_H_





#endif /* SCORE_H_ */