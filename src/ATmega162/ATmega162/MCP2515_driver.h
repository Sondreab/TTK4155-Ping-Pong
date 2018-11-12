/*
 * CAN_driver.h
 *
 * Created: 09.10.2017 11:39:27
 *  Author: marmad
 */ 


#ifndef MCP2515_DRIVER_H_
#define MCP2515_DRIVER_H_



char mcp2515_read(char address);
void mcp2515_write(char address, char data);
void mcp2515_request_to_send(char RTS_command);
void mcp2515_bit_modify(char address, char bitMask, char dataByte);
void mcp2515_reset();
void mcp2515_read_status(char* data);


#endif /* MCP2515_DRIVER_H_ */