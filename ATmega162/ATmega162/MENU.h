/*
 * MENU.h
 *
 * Created: 02.10.2017 16:42:03
 *  Author: sondreab
 */ 

typedef struct menu_t menu_t;

struct menu_t {
	char* name;
	struct menu_t* parent;
	uint8_t num_submenus;
	struct menu_t** submenus;
	};
	
	


menu_t* MENU_init(void);
void MENU_controller(void);
void MENU_home(menu_t home_menu);
void MENU_descend(void);
void MENU_ascend(void);
void MENU_move_cursor(int direction);

#ifndef MENU_H_
#define MENU_H_





#endif /* MENU_H_ */