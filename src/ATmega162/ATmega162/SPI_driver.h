/*
 * SPI_driver.h
 *
 * Created: 09.10.2017 11:13:22
 *  Author: marmad
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_


void SPI_MasterInit(void);
void SPI_send(char cData);
char SPI_read(void);

void SPI_setCSLow();
void SPI_setCSHigh();

#endif /* SPI_DRIVER_H_ */