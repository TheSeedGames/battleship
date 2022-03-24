#include <nds.h>
#include <dswifi9.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl2d.h>
#include <nds/interrupts.h>
#include "board.h"

uint8_t getStatus();
void initSubSprites();
void timer0Callback();
void timer1Callback();
volatile uint32_t timer0Count = 0;

int main() {
	
	//lower the CPU freq
	if (isDSiMode()) setCpuClock(false);
	
	//setup screen modes
	videoSetMode(MODE_5_3D);
	videoSetModeSub(MODE_5_2D);
	
	initBoards();
	initSubSprites();
	setStatus(place);
	swiWaitForVBlank();
	bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	timerStart(0, ClockDivider_64, TIMER_FREQ_64(100), NULL);
	timerStart(1, ClockDivider_256, TIMER_FREQ_256(10), timer1Callback);
	//timerPause(1);
	bool turn = false;
	uint16_t key;
	uint8_t frame = 0;
	glScreen2D();
	while( 1 )
	{
		frame ++;
		scanKeys();
		key = keysDown();
		glBegin2D();
		switch (getStatus()) {
			case menu:
				if(key & KEY_B) setMode(mode_single);
				if(key & KEY_A) setStatus(lobby);
				break;
			case lobby:
				if (~getMode() & 0x02) setStatus(place);
				break;
			case join:
				break;
			case place:
				moveCursor(key);
				if(frame%2) {
					drawBoard(false);
					drawCursor();
				}
				if(key & KEY_A) placeShip();
				break;
			case setup:
				placeShipIA();
				break;
			case play:
				//if (~timer0Count % TIMER_TURN ) turn = !turn;
				moveCursor(key);
				if (frame%2) {
					drawBoard(0);
					drawCursor();
				} else {
					drawBoard(1);
				}
				if(key & KEY_A)
					checkCell();
				break;
			case end:
				break;
		}
		glEnd2D();
		glFlush(0);
		while(REG_DISPCAPCNT & DCAP_ENABLE);
		if (frame % 2)
		{
			//draw botton screen
			lcdMainOnBottom();
			vramSetBankC(VRAM_C_LCD);
			vramSetBankD(VRAM_D_SUB_SPRITE);
			REG_DISPCAPCNT = DCAP_BANK(2) | DCAP_ENABLE | DCAP_SIZE(3);
		}
		else
		{
			//draw top screen
			lcdMainOnTop();
			vramSetBankD(VRAM_D_LCD);
			vramSetBankC(VRAM_C_SUB_BG);
			REG_DISPCAPCNT = DCAP_BANK(3) | DCAP_ENABLE | DCAP_SIZE(3);
		}
		swiWaitForVBlank();
	}
	return 0;
}
void initSubSprites(void) {
	oamInit(&oamSub, SpriteMapping_Bmp_2D_256, false);
	int x = 0;
	int y = 0;
	int id = 0;
	for(y = 0; y < 3; y++)
	for(x = 0; x < 4; x++)
	{
		oamSub.oamMemory[id].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
		oamSub.oamMemory[id].attribute[1] = ATTR1_SIZE_64 | (64 * x);
		oamSub.oamMemory[id].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y) | (8 * x);
		id++;
	}
	swiWaitForVBlank();
	oamUpdate(&oamSub);
}

void timer0Callback(){
	exit(0);
}

void timer1Callback(){
	//exit(5);
}
