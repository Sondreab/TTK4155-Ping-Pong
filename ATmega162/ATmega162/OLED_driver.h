/*
 * OLED_driver.h
 *
 * Created: 25.09.2017 13:46:50
 *  Author: marmad
 */ 

void OLED_init();
void OLED_write_command(char command_line);
void OLED_write_data(char data_byte);
void OLED_reset();
void OLED_goto_line(uint8_t line);
void OLED_clear_line(uint8_t line);
void OLED_pos(uint8_t row, uint8_t col);
void OLED_print_arrow(uint8_t row, uint8_t col);
void OLED_print(char charArray[], size_t size);
void OLED_set_brightness(uint8_t lvl);

void NEW_OLED_print(char* characters);


#ifndef OLED_DRIVER_H_
#define OLED_DRIVER_H_





#endif /* OLED_DRIVER_H_ */