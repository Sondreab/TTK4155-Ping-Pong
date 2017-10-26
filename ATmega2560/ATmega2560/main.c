/*
 * ATmega2560.c
 *
 * Created: 26.10.2017 14:58:32
 *  Author: hploubat
 */ 


#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "UART_driver.h"

int main(void)
{
	char data = "a";
    while(1)
    {
        UART_Transmit(data);
		_delay_ms(500);
    }
}