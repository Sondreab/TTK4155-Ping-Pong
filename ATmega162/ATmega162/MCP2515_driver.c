/*
 * CAN_driver.c
 *
 * Created: 09.10.2017 11:39:10
 *  Author: marmad
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "MCP2515.h"
#include "SPI_driver.h"

void mcp2515_read(char address, char* data) {
	SPI_setCSLow();
	//send the READ instruction 0x03
	SPI_send(MCP_READ);
	//send the address to be read
	SPI_send(address);
	//receive data
	*data = SPI_read();
	SPI_setCSHigh();
}

void mcp2515_write(char address, char data) {
	SPI_setCSLow();
	//Send the WRITE instruction 0x02
	SPI_send(MCP_WRITE);
	//Send address
	SPI_send(address);
	//send data
	SPI_send(data);
	SPI_setCSHigh();
}

void mcp2515_request_to_send(char RTS_command) {
	SPI_setCSLow();
	SPI_send((0x10 | RTS_command));
	SPI_setCSHigh();
}

void mcp2515_bit_modify(char address, char bitMask, char dataByte) {
	SPI_setCSLow();
	SPI_send(MCP_BITMOD);
	SPI_send(address);
	SPI_send(bitMask);
	SPI_send(dataByte);
	SPI_setCSHigh();
	
}

void mcp2515_reset() {
	SPI_setCSLow();
	SPI_send(MCP_RESET);
	SPI_setCSHigh();
}

void mcp2515_read_status(char* data) {
	SPI_setCSLow();
	SPI_send(MCP_READ_STATUS);
	*data = SPI_read();
	SPI_setCSHigh();
}