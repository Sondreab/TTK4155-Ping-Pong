/*
 * EEPROM.c
 *
 * Created: 21.11.2017 01:15:04
 *  Author: marmad
 */ 

#include <avr/io.h>

#define SCORE_LOWBYTE_ADR 0x60
#define SCORE_HIGHBYTE_ADR 0x61

static uint8_t EEPROM_read(uint16_t address){
	//wait for completion of prev write
	while(EECR & (1<<EEWE));
	
	//set up addr register
	EEAR = address;
	
	//start eeprom read by writing EERE
	EECR |= (1<<EERE);
	//return data from data register
	return EEDR;
}

static void EEPROM_write(uint16_t address, uint8_t data){
	//wait for completion of prev write
	while(EECR & (1<<EEWE));
	
	//set up addr register and data register
	EEAR = address;
	EEDR = data;
	
	//write logic one to EEMWE
	EECR |= (1<<EEMWE);
	
	//start eeprom write by setting EEWE
	EECR |= (1<<EEWE);
}

uint16_t EEPROM_read_score(void){
	uint8_t lowbyte = EEPROM_read(SCORE_LOWBYTE_ADR);
	uint8_t highbyte = EEPROM_read(SCORE_HIGHBYTE_ADR);
	uint16_t score = (highbyte<<8);
	score += lowbyte;
	return score;
}

void EEPROM_save_score(uint16_t score){
	uint8_t lowbyte = (score & 0xFF);
	uint8_t highbyte = (score >> 8);
	EEPROM_write(SCORE_LOWBYTE_ADR, lowbyte);
	EEPROM_write(SCORE_HIGHBYTE_ADR, highbyte);
}
