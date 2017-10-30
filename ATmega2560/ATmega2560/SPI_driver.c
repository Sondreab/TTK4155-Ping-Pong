/*
 * SPI_driver.c
 *
 * Created: 09.10.2017 11:13:08
 *  Author: marmad
 */ 


# define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

void SPI_MasterInit(void) {
	//set MOSI and SCK output, and SS, all others input
	DDRB &= ~(1<<PB3);
	DDRB |= (1<<DDB2) | (1<<DDB1) | (1<<DDB7) | (1<<DDB0);
	//enable SPI, master, set clock rate fck/16
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	PORTB |= (1<<PB0);
}

void SPI_send(char cData) {
	//start transmission
	SPDR = cData;
	//wait for transmission to complete
	while(!(SPSR & (1<<SPIF)));
	
}

char SPI_read(void) {
	//send some dummy byte to get a read
	SPDR = 0x00;
	//wait for transmission to complete
	while(!(SPSR & (1<<SPIF)));
	//return the received data
	return SPDR;
}

void SPI_setCSLow() {
	//set /CS low
	PORTB &= ~(1<<PB7);
}

void SPI_setCSHigh() {
	//set /CS high
	PORTB |= (1<<PB7);
}