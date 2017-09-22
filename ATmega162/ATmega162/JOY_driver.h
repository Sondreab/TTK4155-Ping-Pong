/*
 * JOY_driver.h
 *
 * Created: 22.09.2017 14:29:35
 *  Author: sondreab
 */ 



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
	
void JOY_init();
void JOY_calibrate();
int JOY_button(int button);
struct JOY_position_t JOY_getPosition();
enum JOY_direction_t JOY_getDirection();


#ifndef JOY_DRIVER_H_
#define JOY_DRIVER_H_





#endif /* JOY_DRIVER_H_ */