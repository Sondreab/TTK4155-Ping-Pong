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

enum JOY_button_t{
	JOY_BUTTON,
	L_BUTTON,
	R_BUTTON,
};

	
struct JOY_sliders_t {
	int L_slider;
	int R_slider;
};

struct JOY_data_t {
	struct JOY_position_t position;
	
	enum JOY_direction_t direction;
	
	struct JOY_sliders_t sliders;
	
	int joy_button;
	int L_button;
	int R_button;
		
};
	
void JOY_init();
void JOY_calibrate();
//int JOY_button(int button); OLD -> WORKING
int JOY_button(enum JOY_button_t button); //increased code quality
struct JOY_position_t JOY_getPosition();
enum JOY_direction_t JOY_getDirection();
struct JOY_sliders_t JOY_getSliderPosition();
void JOY_initialize_state(struct JOY_data_t *state);
int JOY_poll_change(struct JOY_data_t *prev, struct JOY_data_t *curr);
void JOY_loopedTest();


#ifndef JOY_DRIVER_H_
#define JOY_DRIVER_H_





#endif /* JOY_DRIVER_H_ */