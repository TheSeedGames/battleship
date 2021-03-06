#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdlib.h>

extern enum play_status {
	menu,
	lobby,
	join,
	place,
	setup,
	play,
	end
} play_status;

extern enum gameMode {
	mode_debug = 0,
	mode_single,
	mode_host,
	mode_join
} gameMode;
/*
extern enum screens {
	screen_upper,
	screen_lower
} screens;
*/
extern enum cell_status {
	CELL_UNKNOWN = 0,
	CELL_WATER,
	CELL_SHIP_INTACT,
	CELL_SHIP_HIT,
	CELL_SHIP_DAMAGED,
	CELL_SHIP_SUNKEN
} cell_status;


typedef struct match_t {
	uint16_t mode:2;
	uint16_t vertical:1;
	uint16_t gameStatus:3;
} match_t;

typedef struct cell_t {
	uint8_t ship_ID: 4;
	uint8_t status: 3;
	uint8_t discovered: 1;
} cell_t;

typedef struct ship_t {
	uint16_t placed: 1;
	uint16_t position:7;
	uint16_t size: 3;
	uint16_t integrity: 3;
	uint16_t vertical:1;
	uint16_t destroyed: 1;
} ship_t;



#endif
