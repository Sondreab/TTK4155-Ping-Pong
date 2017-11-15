/*
 * Motor_controller.h
 *
 * Created: 13.11.2017 14:10:41
 *  Author: hploubat
 */ 

#include "DAC_driver.h"

void Motor_init();
void Set_Motor(int16_t speed);
void Motor_disable();
int16_t Get_motor_pos();
void Fire_solenoid();
void retract_solenoid();

#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_





#endif /* MOTOR_CONTROLLER_H_ */