/*
 * CAN_driver.h
 *
 * Created: 09.10.2017 13:35:38
 *  Author: marmad
 */ 

#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_



struct CAN_message{
	unsigned int id;
	uint8_t length;
	uint8_t data[8];
	};


void CAN_init();
void CAN_message_send();
void CAN_error();
void CAN_transmit_complete();
void CAN_data_recieve();
void CAN_int_vect();

#endif /* CAN_DRIVER_H_ */

