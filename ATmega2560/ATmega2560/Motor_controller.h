/*
 * Motor_controller.h
 *
 * Created: 13.11.2017 14:10:41
 *  Author: hploubat
 */ 

#include "DAC_driver.h"

void Motor_init();
void Set_Motor(uint8_t direction, uint8_t speed);
void Get_motor_pos();
void Fire_solenoid();

#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_





#endif /* MOTOR_CONTROLLER_H_ */