/*
 * typedef.h
 *
 * Created: 30.10.2017 15:28:21
 *  Author: hploubat
 */ 

typedef enum{
	MOTOR_POS,
	SERVO,
	SOLENOID,	
	}CAN_data_byte_t;


typedef enum{
	X_JOY = 1,
	Y_JOY,
	L_SLIDER,
	R_SLIDER,
}ADC_CHANNEL_t;

typedef enum{
	JOY_BUTTON,
	L_BUTTON,
	R_BUTTON,
}JOY_button_t;

typedef struct{
	ADC_CHANNEL_t motor_pos;
	ADC_CHANNEL_t servo;
	JOY_button_t solenoid;
}USER_controls_t;

typedef struct{
	char* name;
	uint16_t highscore;
	USER_controls_t ctrl_pref;
}USER_profile_t;

typedef enum{
	NO_ACTION,
	MENU,
	PLAY_GAME,
	SET_BRIGHTNESS,
	CAL_JOY,
	GAME_OVER,
	CTRL_SETTINGS,
	SET_MOTOR_POS_R_SLIDER,
	SET_MOTOR_POS_L_SLIDER,
	SET_MOTOR_POS_X_JOY,
	SET_MOTOR_POS_Y_JOY,
	SET_SERVO_X_JOY,
	SET_SERVO_Y_JOY,
	SET_SERVO_L_SLIDER,
	SET_SERVO_R_SLIDER,
	SET_SOLENOID_R_BUTTON,
	SET_SOLENOID_L_BUTTON,
	SET_SOLENOID_JOY_BUTTON,
}STATE_t;



#ifndef TYPEDEF_H_
#define TYPEDEF_H_





#endif /* TYPEDEF_H_ */