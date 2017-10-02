/*
 * UART_driver.h
 *
 * Created: 11.09.2017 11:07:39
 *  Author: hploubat
 */ 



void UART_Init( unsigned int ubrr );
void UART_Transmit(char data);
int UART_Receive(void);
void UART_Test(void);

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_





#endif /* UART_DRIVER_H_ */