/*
 * main.c
 *
 * Created: 11.09.2017 11:06:58
 *  Author: hploubat
 */ 



# define F_CPU 4915200UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "UART_driver.h"
#include "XMEM_driver.h"
#include "JOY_driver.h"
#include "ADC_driver.h"
#include "OLED_driver.h"
#include "MENU.h"
#include "CAN_driver.h"


#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1


int main(void){
	
	UART_Init ( MYUBRR );
	fdevopen(&UART_Transmit, &UART_Receive);
	
	INTR_init();
	XMEM_init();
	XMEM_test();
	OLED_init();
	OLED_reset();
	CAN_init();
	
	struct CAN_msg_t msg;
	msg.id = 3;
	msg.length = 1;
	msg.data[0] = 3;
	printf("Before send: %i\n", msg.data[0]);
	CAN_message_send(&msg);
	struct CAN_msg_t message = CAN_data_recieve();
	
	printf("%i", message.data[0]);
	
	//mcp2515_write(0b00110110,0x55);
	//uint8_t temp = mcp2515_read(0b00110110);
	//printf("Test %x\n",temp);
	//OLED_pos(0,0);
 	//menu_t* mainMenu = MENU_init();
 	//MENU_controller();
	//JOY_loopedTest();

}

