/*
 * Motor_controller.h
 *
 * Created: 13.11.2017 14:10:41
 *  Author: hploubat
 */ 


void BOARD_motor_init();
int static BOARD_motor_to_edge(int direction);
int BOARD_initialize_for_game();
void BOARD_set_motor(int16_t speed);
void BOARD_motor_disable();
int16_t BOARD_get_motor_pos();
void BOARD_fire_solenoid();
void BOARD_retract_solenoid();


#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_





#endif /* MOTOR_CONTROLLER_H_ */