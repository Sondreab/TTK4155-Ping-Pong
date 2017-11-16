/*
 * JOY_driver.c
 *
 * Created: 22.09.2017 14:29:19
 *  Author: sondreab
 */ 

# define F_CPU 4915200UL
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include "ADC_driver.h"
#include "JOY_driver.h"



//Set in JOY_calibrate()
static volatile int Rx_max = 255;
static volatile int Rx_min = 0;
static int Range_x = 255;
static int Mean_x = 127;

static volatile int Ry_max = 255;
static volatile int Ry_min = 0;
static int Range_y = 255;
static int Mean_y = 127;


void JOY_init(){
	DDRB &= ~(1<<PB0);
	DDRB &= ~(1<<PB1);
	DDRB &= ~(1<<PB2);
	printf("Calibrate Joystick?\n");
	printf("Press RIGHT touch button to calibrate or LEFT touch button to skip.\n");
	while(1){
		if(JOY_button(2)){
			printf("Entering Calibration.\n");
			JOY_calibrate();
			break;
		}else if(JOY_button(1)){
			break;
		}
	}
}

void JOY_calibrate() {
	printf("Hold joystick to the LEFT, then press the LEFT touch button\n");
	while(1) {
		if(JOY_button(1)) {
			Rx_min = ADC_read(1);
			break;
		}
	}
	
	printf("Hold joystick to the RIGHT, then press the RIGHT touch button\n");
	while(1) {
		if(JOY_button(2)) {
			Rx_max = ADC_read(1);
			break;
		}
	}
	
	printf("Hold joystick DOWN, then press LEFT touch button\n");
	while(1) {
		if(JOY_button(1)) {
			Ry_min = ADC_read(2);
			break;
		}
	}
	
	printf("Hold joystick UP, then press the RIGHT touch button\n");
	while(1) {
		if(JOY_button(2)) {
			Ry_max = ADC_read(2);
			break;
		}
	}
	
	Range_x = Rx_max - Rx_min;
	Range_y = Ry_max - Ry_min;
	Mean_x = Range_x / 2;
	Mean_y = Range_y / 2;
	
	printf("Joystick X:  ( %i - %i - %i ) %i\n", Rx_min, Mean_x, Rx_max, Range_x);
	printf("Joystick Y:  ( %i - %i - %i ) %i\n", Ry_min, Mean_y, Ry_max, Range_y);
}

int JOY_button(enum JOY_button_t button) {
	int output = 0;
	//Button = 0 -> Joystick Button
	//button = 1 -> Left touchbutton (PB0)
	//button = 2 -> Right touchbutton (PB1)
	switch(button){
		case JOY_BUTTON:
		if(!(PINB & (1<<PB0))){
			output = 1;
		}
		break;
		case L_BUTTON:
		if(PINB & (1<<PB1)){
			output = 1;
		}
		break;
		case R_BUTTON:
		if(PINB & (1<<PB2)){
			output = 1;
		}
		break;
		default:
		output = -1;
		
	}
	return output;
}

struct JOY_position_t JOY_getPosition() {
	struct JOY_position_t position;
	
	int x_ADC = ADC_read(1);
	float temp = (x_ADC - Mean_x)*100;
	temp = temp/(Range_x/2);
	//converting from 0-255 to -100 to 100
	position.X = temp;
	
	int y_ADC = ADC_read(2);
	temp = (y_ADC - Mean_y)*100;
	temp = temp/(Range_y/2);
	//converting from 0-255 to -100 to 100
	position.Y = temp;
	
	return position;
}

enum JOY_direction_t JOY_getDirection() {
	enum JOY_direction_t direction; 
	struct JOY_position_t position = JOY_getPosition();
	if (abs(position.X)>abs(position.Y)){
		if (position.X > 0) {
			direction = RIGHT;
		}else{
			direction = LEFT;
		}
		
	}
	else {
		if (position.Y > 0) {
			direction = UP;
		}else{
			direction = DOWN;
		}
		
	}
	//change the value for the comparison to change the dead-zone in the middle
	if (abs(position.X)<20 && abs(position.Y)<20) {
		direction = NEUTRAL;
	}
	
	return direction;
}

struct JOY_sliders_t JOY_getSliderPosition(){
	struct JOY_sliders_t sliders;
	
	sliders.L_slider = ADC_read(3);
	sliders.R_slider = ADC_read(4);
	
	return sliders;
}

void JOY_initialize_state(struct JOY_data_t *state){
	state->position = JOY_getPosition();
	state->direction = JOY_getDirection();
	state->sliders = JOY_getSliderPosition();
	state->joy_button = JOY_button(JOY_BUTTON);
	state->L_button = JOY_button(L_BUTTON);
	state->R_button = JOY_button(R_BUTTON);

}

int JOY_poll_change(struct JOY_data_t *prev, struct JOY_data_t *curr){
	prev->position = curr->position;
	prev->direction = curr->direction;
	prev->sliders = curr->sliders;
	prev->joy_button = curr->joy_button;
	prev->L_button = curr->L_button;
	prev->R_button = curr->R_button;
	
	curr->position = JOY_getPosition();
	curr->direction = JOY_getDirection();
	curr->sliders = JOY_getSliderPosition();
	curr->joy_button = JOY_button(JOY_BUTTON);
	curr->L_button = JOY_button(L_BUTTON);
	curr->R_button = JOY_button(R_BUTTON);
	
	if (prev->position.X != curr->position.X){
		return 1;
	}else if (prev->position.Y != curr->position.Y){
		return 1;
	}else if (prev->direction != curr->direction){
		return 1;
	}else if (prev->sliders.L_slider != curr->sliders.L_slider){
		return 1;
	}else if (prev->sliders.R_slider != curr->sliders.R_slider){
		return 1;
	}else if (prev->joy_button != curr->joy_button){
		return 1;
	}else if (prev->L_button != curr->L_button){
		return 1;
	}else if (prev->R_button != curr->R_button){
		return 1;
	}
	
	return 0;	
}


void JOY_loopedTest(){
	while(1){
		struct JOY_position_t position = JOY_getPosition();
		enum JOY_direction_t direction = JOY_getDirection();
		struct JOY_sliders_t sliders = JOY_getSliderPosition();
		printf("( %i , %i ) - DIR:  %i \n", position.X, position.Y, direction);
		printf("Slider: (%i, %i)\n", sliders.L_slider, sliders.R_slider);
		printf("ButtonL: %i \nButtonR: %i\n JOYButton:%i\n", JOY_button(1), JOY_button(2), JOY_button(0));
		
		_delay_ms(1000);
	}
}