/*
 * MENU.c
 *
 * Created: 02.10.2017 16:42:16
 *  Author: sondreab
 */ 

# define F_CPU 4915200UL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "MENU.h"
#include "OLED_driver.h"
#include "JOY_driver.h"

static menu_t* mainMenu;
static menu_t* settings;
static uint8_t menu_initialized = 0;

menu_t* new_menu(char* name, uint8_t num_submenus, STATE_t state){
	menu_t* m = malloc(sizeof(menu_t));
	if(num_submenus){
		m->submenus = malloc(sizeof(menu_t*) * num_submenus);
	}
	m->num_submenus = num_submenus;
	m->name = name;
	m->state = state;
	//leave space here for a function-pointer!
	
	return m;
}

static void menu_assign_parents(menu_t* m){
	for(uint8_t i = 0; i < m->num_submenus; i++) {
		m->submenus[i]->parent = m;
		menu_assign_parents(m->submenus[i]);
	}
}

void menu_create(void) {
	mainMenu = new_menu("Main Menu", 2, NO_ACTION); 
	mainMenu->submenus[0] = new_menu("Play game", 0, PLAY_GAME);
	
	mainMenu->submenus[1] = new_menu("Settings", 1, NO_ACTION);
	settings = mainMenu->submenus[1];
	
	settings->submenus[0] = new_menu("Brightness", 0, SET_BRIGHTNESS);
	
	
	//mainMenu->submenus[2] = new_menu("Highscores", 0, NULL);
	
	
	menu_assign_parents(mainMenu);
}

menu_t* MENU_init(void) {
	if(!menu_initialized) {
		menu_create();
		menu_initialized = 1;
	}
	return mainMenu;
}




STATE_t MENU_controller(menu_t* menu_ptr){
	STATE_t state = NO_ACTION;
	printf("%i\n",state);
	OLED_reset();
	NEW_OLED_print(menu_ptr->name);
	for (uint8_t i = 0; i < menu_ptr->num_submenus; i++) {
		OLED_pos((i+1), 0);
		NEW_OLED_print("     ");
		NEW_OLED_print(menu_ptr->submenus[i]->name);
		
	}
	uint8_t arrowPos = 1;
	OLED_print_arrow(arrowPos,0);
	enum JOY_direction_t input;
	while(state == NO_ACTION) {
		if (arrowPos >0) {
			OLED_print_arrow(arrowPos,0);
		}
		input = JOY_getDirection();
		switch(input) {
			case DOWN:
				if (arrowPos == menu_ptr->num_submenus) break;
				OLED_pos(arrowPos,0);
				NEW_OLED_print("     ");
				arrowPos++;
				break;
			case UP:
				if (arrowPos ==1) break;
				OLED_pos(arrowPos,0);
				NEW_OLED_print("     ");
				arrowPos--;
				break;
			default:
				break;
		
		}
		if (JOY_button(JOY_BUTTON)) {
			
			if(menu_ptr->submenus[arrowPos-1]->num_submenus != 0){
				state = MENU_controller(menu_ptr->submenus[arrowPos-1]);
			}else{
				state = menu_ptr->submenus[arrowPos-1]->state;
			}
		}
		_delay_ms(200);
	}
	
	return state;	
}


