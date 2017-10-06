/*
 * OLED_driver.c
 *
 * Created: 25.09.2017 13:52:18
 *  Author: marmad
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include "OLED_driver.h"
#include "fonts.h"


void OLED_init(){
	OLED_write_command(0xae);       //display  off
	OLED_write_command(0xa1);       //segment  remap
	OLED_write_command(0xda);       //common  pads  hardware:  alternative
	OLED_write_command(0x12);
	OLED_write_command(0xc8);       //common output scan direction:com63~com0
	OLED_write_command(0xa8);       //multiplex  ration  mode:63
	OLED_write_command(0x3f);
	OLED_write_command(0xd5);       //display divide ratio/osc. freq. mode
	OLED_write_command(0x80);
	OLED_write_command(0x81);       //contrast  control
	OLED_write_command(0x50);
	OLED_write_command(0xd9);       //set  pre-charge  period
	OLED_write_command(0x21);
	OLED_write_command(0x20);       //Set  Memory  Addressing  Mode
	OLED_write_command(0x02);
	OLED_write_command(0xdb);       //VCOM  deselect  level  mode
	OLED_write_command(0x30);
	OLED_write_command(0xad);       //master  configuration
	OLED_write_command(0x00);
	OLED_write_command(0xa4);       //out  follows  RAM  content
	OLED_write_command(0xa6);       //set  normal  display
	OLED_write_command(0xaf);		// display on 
}

void OLED_write_command(char command_byte){
	volatile char *command_address = 0x1000;
	*command_address = command_byte;
}

void OLED_write_data(char data_byte){
	volatile char *data_address = 0x1200;
	*data_address = data_byte;
}

void OLED_reset(){
	for(uint8_t line = 0; line <= 7; line++){
		OLED_clear_line(line);
	}
}

void OLED_goto_line(uint8_t line){
	OLED_write_command((0xB0) | line);
	
}

void OLED_clear_line(uint8_t line){
	OLED_pos(line, 0x00);
	for(uint8_t col = 0; col <= 127; col++){
		OLED_write_data(0x00);
	}
}

void OLED_pos(uint8_t row, uint8_t column){
	OLED_goto_line(row);
	OLED_write_command((0x0F) & column);
	OLED_write_command((((0xF0) & column) >> 4) | 0x10);
}

void OLED_print_arrow(uint8_t row, uint8_t col){
	OLED_pos(row, col);
	OLED_write_data(0b00011000);
	OLED_write_data(0b00011000);
	OLED_write_data(0b01111110);
	OLED_write_data(0b00111100);	
	OLED_write_data(0b00011000);
}

void OLED_print(char charArray[], size_t size){
	for(int element = 0; element < size-1; element++){
		char character = charArray[element];
		if (character == '\0'){
			break;
		}
		character = character - 32;
		uint8_t byte;
		for(int col = 0; col < 5; col++){
			byte = pgm_read_byte(&(font5[character][col]));
			OLED_write_data(byte);
		}
	}
	
}

void OLED_set_brightness(uint8_t lvl){
	
}