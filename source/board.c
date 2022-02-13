#include "board.h"
#include "data_types.h"


match_t flags;
 
cell_t *board_upper;
cell_t *board_lower;
cell_t *player_board;
cell_t *oponents_board;
ship_t *ships;
ship_t * ship;

void initBoards() {
	board_lower = (cell_t* ) malloc(sizeof(cell_t) * BOARD_CELLS);
	board_upper = (cell_t* ) malloc(sizeof(cell_t) * BOARD_CELLS);
	memset(board_upper,CELL_UNKNOWN,BOARD_CELLS);
	memset(board_lower,CELL_UNKNOWN,BOARD_CELLS);
	player_board = board_lower;
	oponents_board = board_upper;

}

void switchBoards(){
	player_board = board_upper;
	oponents_board = board_lower;
}

void drawBoard(bool screen){
	cell_t *board = board_lower;
	if (screen) board = board_upper;
	//BACKGROUND
	glBoxFilled(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,DARK_GREY);
	//TITLE
	glBoxFilled(4, 4, SCREEN_WIDTH - 4, 24, LIGHT_GREY);
	//BOARD BACKGROUND
	glBoxFilled(4,28,BOARD_PIXELS_SIDE + 4, BOARD_PIXELS_SIDE + 28, WHITE);
	//BOARD GRID
	for (uint8_t cell = 0; cell < 100; cell++) {
		uint8_t x = cell % 10 * 16 + 4;
		uint8_t y = cell / 10 * 16 + 28;
		uint16_t color = BLACK;
		uint8_t state = board[cell].status;
		switch (state){
			case CELL_UNKNOWN:
				color = GREY;
				break;
			case CELL_WATER:
				color = BLUE;
				break;
			case CELL_SHIP_INTACT:
				color = GREEN;
		 		break;
			case CELL_SHIP_HIT:
				color = ORANGE;
		 		break;
			case CELL_SHIP_DAMAGED:
				color = RED;
				break;
			case CELL_SHIP_SUNKEN:
				color = BLACK;
				break;
			}
		glBoxFilled(x+1,y+1,x+15,y+15,color);
	}
}


void drawCursor() {
	uint8_t size = ship->size;
	uint8_t pos = flags.active_cell;
	bool vert = flags.vertical;
	uint8_t x = (pos % 10)*16;
	uint8_t y = (pos / 10)*16;
	if(vert)
		for (uint8_t sec = 0; sec < size; sec ++) {
			glBox(x + 5, y + 29 + sec * 16, x + 18, y + 42 + sec * 16, ORANGE);
			glBox(x + 4, y + 28 + sec * 16, x + 19, y + 43 + sec * 16, ORANGE);
		}
	else
		for (uint8_t sec = 0; sec < size; sec ++) {
			glBox(x + 5 + sec * 16, y + 29, x + 18 + sec * 16, y + 42, ORANGE);
			glBox(x + 4 + sec * 16, y + 28, x + 19 + sec * 16, y + 43, ORANGE);
		}
}

void drawShips() {
	//SCOREBOARD BACKGROUND
	glBoxFilled(BOARD_PIXELS_SIDE + 8, 28, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 4,LIGHT_GREY);
	//SHIPS
	glBoxFilled(BOARD_PIXELS_SIDE + 18, 32, BOARD_PIXELS_SIDE + 18 + CELL_SIDE_PIXELS * 4, 48, GREEN);
}

bool placeShip() {
	uint8_t step = flags.vertical ? 10 : 1;
	uint8_t pos = flags.active_cell;
	uint8_t size = ship->size;
	cell_t *board = board_lower;
	for (uint8_t cell = pos; cell < (pos + size*step); cell += step)
		if (board[cell].status != CELL_UNKNOWN) return false;
	for (uint8_t cell = pos; cell < (pos + size*step); cell += step) 
	{	
		uint8_t x = cell % 10;
		if(x) {
			if (cell > 10) board[cell-11].status = CELL_WATER;
			board[cell-1].status = CELL_WATER;
			if (cell < 90) board[cell+9].status = CELL_WATER;
		}
		if(x !=9) {
			if (cell > 10) board[cell-9].status = CELL_WATER;
			board[cell+1].status = CELL_WATER;
			if (cell < 90) board[cell+11].status = CELL_WATER;
		}
		if (cell > 10) board[cell-10].status = CELL_WATER;
		if (cell < 90 ) board[cell+10].status = CELL_WATER;
	}
	for (uint8_t cell = pos; cell < (pos + size*step); cell += step) 
		board[cell].status = CELL_SHIP_INTACT;
	return true;
}
inline uint8_t checkCell() 
{
	return oponents_board[flags.active_cell].status;
}

void drawMenu() {
	
}

void initShips() {
	ships[0] = (ship_t){.size = 4,.id = 0};
	ships[1] = (ship_t){.size = 3,.id = 1};
	ships[2] = (ship_t){.size = 3,.id = 2};
	ships[3] = (ship_t){.size = 2,.id = 3};
	ships[4] = (ship_t){.size = 2,.id = 4};
	ships[5] = (ship_t){.size = 2,.id = 5};
	ships[6] = (ship_t){.size = 1,.id = 6};
	ships[7] = (ship_t){.size = 1,.id = 7};
	ships[8] = (ship_t){.size = 1,.id = 8};
	ships[9] = (ship_t){.size = 1,.id = 9};
}

void moveCursor(uint16_t key) {
	if(key & KEY_B) flags.vertical ++;
	if(key & KEY_UP)    flags.active_cell = (flags.active_cell + 90) % 100;
	if(key & KEY_DOWN)  flags.active_cell = (flags.active_cell + 10) % 100;
	if(key & KEY_LEFT)  flags.active_cell = (((flags.active_cell % 10) + 9) % 10) + (flags.active_cell / 10) * 10;
	if(key & KEY_RIGHT) flags.active_cell = (((flags.active_cell % 10) + 1) % 10) + (flags.active_cell / 10) * 10;
	if(flags.vertical) {
		if(flags.active_cell / 10 > 10 - ship->size) while (flags.active_cell / 10 > 10 - ship->size)  flags.active_cell -= 10;
	} else { 
		if (flags.active_cell % 10 > 10 - ship->size) while(flags.active_cell % 10 > 10 - ship->size) flags.active_cell -= 1; 
	}
}

inline uint8_t getStatus() {
	return flags.gameStatus;
}

inline uint8_t getMode() {
	return flags.mode;
}

void setStatus(uint8_t status) {
		flags.gameStatus=status;
}

void setMode(uint8_t mode) {
		flags.mode=mode;
}

