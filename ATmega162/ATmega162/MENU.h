/*
 * MENU.h
 *
 * Created: 02.10.2017 16:42:03
 *  Author: sondreab
 */ 



typedef struct menu_t{
	char name[11];
	struct menu_t* parent;
	struct menu_t* contents[6]; 
	} menu_t;
	
	
struct menu_t *main_menu = NULL;
struct menu_t *play_game = NULL;
struct menu_t *settings = NULL;
struct menu_t *highscores = NULL;

void MENU_create(char name[], menu_t * parent, menu_t * contents[]); 
int MENU_init(void);
void MENU_controller(void);
void MENU_home(menu_t home_menu);
void MENU_descend(void);
void MENU_ascend(void);
void MENU_move_cursor(int direction);

#ifndef MENU_H_
#define MENU_H_





#endif /* MENU_H_ */