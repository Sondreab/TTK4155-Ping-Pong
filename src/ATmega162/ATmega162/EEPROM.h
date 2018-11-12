/*
 * EEPROM.h
 *
 * Created: 21.11.2017 01:15:17
 *  Author: marmad
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

static uint8_t EEPROM_read(uint16_t address);
static void EEPROM_write(uint16_t address, uint8_t data);
uint16_t EEPROM_read_score(void);
void EEPROM_save_score(uint16_t data);




#endif /* EEPROM_H_ */