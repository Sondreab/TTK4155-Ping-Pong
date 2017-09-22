/*
 * JOY_driver.c
 *
 * Created: 22.09.2017 14:29:19
 *  Author: sondreab
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "ADC_driver.h"
#include "JOY_driver.h"

//Set in JOY_calibrate()
static int Rx_max = 255;
static int Rx_min = 0;
static int Range_x = 255;
static int Mean_x = 127;

static int Ry_max = 255;
static int Ry_min = 0;
static int Range_y = 255;
static int Mean_y = 127;


void JOY_init();

void JOY_calibrate() {
	printf("Hold joystick to the left, then press button 1\n");
	while(1) {
		if(JOY_button(1)) {
			Rx_min = ADC_read(1);
		}
	}
	
	printf("Hold joystick to the right, then press button 1\n");
	while(1) {
		if(JOY_button(1)) {
			Rx_max = ADC_read(1);
		}
	}
	
	printf("Hold joystick down, then press button 1\n");
	while(1) {
		if(JOY_button(1)) {
			Ry_min = ADC_read(2);
		}
	}
	
	printf("Hold joystick up, then press button 1\n");
	while(1) {
		if(JOY_button(1)) {
			Ry_max = ADC_read(2);
		}
	}
	
	//Range_x = Rx_max - Rx_min
}

int JOY_button(int button) {
	
}

struct JOY_position_t JOY_getPosition() {
	struct JOY_position_t position;
	
	int x_ADC = ADC_read(1);
	//converting from 0-255 to -100 to 100
	position.X = (x_ADC - Mean_x)/(Range_x/100);
	//printf("%i \n", (int)((x_ADC - Mean_x)/(Range_x/100)));
	
	int y_ADC = ADC_read(2);
	//converting from 0-255 to -100 to 100
	position.Y = (y_ADC - Mean_y)/(Range_y/100);
	
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
	if (abs(position.X)<5 && abs(position.Y)<5) {
		direction = NEUTRAL;
	}
	
	return direction;
}