/*
 * JOY_driver.h
 *
 * Created: 22.09.2017 14:29:35
 *  Author: sondreab
 */ 

#define NUM_BUTTONS 3

// enum JOY_button_t {
// 	JOYSTICK_BUTTON,
// 	LEFT_BUTTON,
// 	RIGHT_BUTTON,
// 	};


struct JOY_position_t {
	int X;
	int Y;
	};

enum JOY_direction_t {
	NEUTRAL,
	UP,
	RIGHT,
	DOWN,
	LEFT,
	};
	
struct JOY_sliders_t {
	int L_slider;
	int R_slider;
};
	
void JOY_init();
void JOY_calibrate();
int JOY_button(int button);
struct JOY_position_t JOY_getPosition();
enum JOY_direction_t JOY_getDirection();
struct JOY_sliders_t JOY_getSliderPosition();
void JOY_loopedTest();


#ifndef JOY_DRIVER_H_
#define JOY_DRIVER_H_





#endif /* JOY_DRIVER_H_ */