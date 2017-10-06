/*
 * MENU.c
 *
 * Created: 02.10.2017 16:42:16
 *  Author: sondreab
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MENU.h"
#include "OLED_driver.h"

void MENU_create(char name[], menu_t * parent, menu_t * contents[]){
	struct menu_t *menu = malloc(sizeof(struct menu_t));
	if(!menu){
		exit(1);
	}
	strncpy(menu->name, name, sizeof(menu->name));
	menu->parent = parent;
	for(int element = 0; element < sizeof(contents)/sizeof(contents[0]); element++){
		menu->contents[element] = contents[element];
	}
}

int MENU_init(void){
	play_game = malloc(sizeof(struct menu_t));
	if(!play_game){
		exit(1);
	}
	strncpy(play_game->name, "Play Game", sizeof(play_game->name));
	play_game->parent = NULL;
	
	settings = malloc(sizeof(struct menu_t));
	if(!settings){
		exit(1);
	}
	strncpy(settings->name, "Settings",sizeof(settings->name));
	settings->parent = NULL;
	
	
	highscores = malloc(sizeof(struct menu_t));
	if(!highscores){
		exit(1);
	}
	strncpy(highscores->name, "Highscores", sizeof(highscores->name));
	highscores->parent = NULL;

	main_menu = malloc(sizeof(struct menu_t));
	if(!main_menu){
		exit(1);
	}
	strncpy(main_menu->name, "Main Menu", sizeof(main_menu->name));
	main_menu->parent = NULL;
	main_menu->contents[0] = play_game;
	main_menu->contents[1] = highscores;
	main_menu->contents[1] = settings;
	
	play_game->parent = main_menu;
	highscores->parent = main_menu;
	settings->parent = main_menu;	
	
}

void MENU_controller(void){
	
	
}

void MENU_home(menu_t home_menu){
	OLED_pos(0,5);
	OLED_print(home_menu.name, sizeof(home_menu.name));
	for(int element = 0; element <= sizeof(home_menu.contents)/sizeof(home_menu.contents[0]); element++){
		OLED_pos(element+1,5);
		OLED_print(home_menu.contents[element]->name, sizeof(home_menu.contents[element]->name));
	}
}

void MENU_descend(void){
	
}

void MENU_ascend(void){
	
}

void MENU_move_cursor(int direction){
	
}

