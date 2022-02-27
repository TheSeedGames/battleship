#ifndef BOARD_H
#define BOARD_H

#include "data_types.h"
#include <nds.h>
#include <stdio.h>
#include <gl2d.h>

#define BLACK		RGB5(0,0,0)
#define WHITE 		RGB5(31,31,31)
#define GREY 		RGB5(15,15,15)
#define DARK_GREY 	RGB5(7,7,7)
#define LIGHT_GREY 	RGB5(23,23,23)
#define WHITE 		RGB5(31,31,31)
#define DARK_BLUE 	RGB5(0,0,7)
#define BLUE 		RGB5(0,0,15)
#define GREEN 		RGB5(0,15,0)
#define ORANGE 		RGB5(31,12,2)
#define YELLOW 		RGB5(30,30,0)
#define RED 		RGB5(15,0,0)
#define LIGHT_BLUE 	RGB5(7,7,31)

#define BOARD_SIDE_CELLS	10
#define BOARD_CELLS	100
#define CELL_SIDE_PIXELS	16
#define BOARD_PIXELS_SIDE CELL_SIDE_PIXELS*BOARD_SIDE_CELLS

#define HALF_WIDTH (SCREEN_WIDTH/2)
#define HALF_HEIGHT (SCREEN_HEIGHT/2)
#define BRAD_PI (1 << 14)

void fillWater(cell_t* board);
void moveCursor(uint16_t keys);
void initBoards();
void initShips();
void drawBoard(bool screen);
void placeShip ();
void drawShips();
void drawMenu();
bool checkWin();
bool checkLose();
void switchBoards();
uint8_t checkCell();
void drawCursor();
uint8_t getStatus();
void setStatus(uint8_t status);
uint8_t getMode();
void setMode(uint8_t mode);

#endif
