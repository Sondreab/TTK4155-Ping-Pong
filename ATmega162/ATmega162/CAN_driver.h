/*
 * CAN_driver.h
 *
 * Created: 09.10.2017 13:35:38
 *  Author: marmad
 */ 

#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

typedef struct CAN_msg_t{
	unsigned int id;
	uint8_t length;
	uint8_t data[8];
	}; 

uint8_t CAN_init();
void CAN_message_send(struct CAN_msg_t* msg);
void CAN_error();
void CAN_data_recieve(struct CAN_msg_t* msg);

// void CAN_transmit_complete();
// void CAN_int_vect();

#endif /* CAN_DRIVER_H_ */

