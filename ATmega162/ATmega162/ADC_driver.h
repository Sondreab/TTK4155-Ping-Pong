/*
 * ADC_driver.h
 *
 * Created: 18.09.2017 13:48:36
 *  Author: hploubat
 */ 

typedef enum{
	X_JOY = 1,
	Y_JOY,
	L_SLIDER,
	R_SLIDER,	
}ADC_CHANNEL_t;

void INTR_init(void);
uint8_t ADC_read(uint8_t channel);


#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_





#endif /* ADC_DRIVER_H_ */