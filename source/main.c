#include <nds.h>
#include <dswifi9.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl2d.h>
#include "board.h"

#define HALF_WIDTH (SCREEN_WIDTH/2)
#define HALF_HEIGHT (SCREEN_HEIGHT/2)
#define BRAD_PI (1 << 14)


uint8_t getStatus();
void initSubSprites();

int main() {
	
	//lower the CPU freq
	if (isDSiMode()) setCpuClock(false);
	
	//setup screen modes
	videoSetMode(MODE_5_3D);
	videoSetModeSub(MODE_5_2D);
	
	initBoards();
	initShips();
	initSubSprites();
	setStatus(place);
	// sub background holds the top image when 3D directed to bottom
	// init oam to capture 3D scene
	swiWaitForVBlank();
	bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	/*
	NF_SetRootFolder("NITROFS");
 	NF_InitTiledBgBuffers();
 	NF_InitTiledBgSys(0);
	NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);
	NF_CreateTextLayer(0, 0, 0,	"normal");
	NF_WriteText(0, 0, 1, 1, "Hola Mundo!\n Hello World!");	// Normal
	
	NF_UpdateTextLayers();*/



	uint16_t key;		// for key input
	uint8_t frame = 0;
	
/*
	char dbg[256];
	uint8_t nameLenght = PersonalData->nameLen;
	uint16_t *nameUTF = malloc(nameLenght);
	char *nameASCII = malloc(nameLenght);
	memcpy(nameUTF,PersonalData->name,nameLenght*2);
	for(uint8_t u = 0; u < nameLenght; u++) {
		if (nameUTF[u*2]) break;
		if (nameUTF[u*2+1] & 0x80) break;
		nameASCII[u] = nameUTF[u*2];
	}
	nocashMessage (nameASCII);
	*/
	//consoleDemoInit();
	//iprintf(nameASCII);
	glScreen2D();
	//powerOn(POWER_ALL);
	while( 1 )
	{
		frame ++;
		//read keys
		scanKeys();
		key = keysDown();
		switch (getStatus()) {
			case menu:
				if(key & KEY_B) setMode(mode_single);
				if(key & KEY_A) setStatus(lobby);
				break;
			case lobby:
				if (!(getMode() & 0x02)) setStatus(place);
				break;
			case join:
				break;
			case place:
				moveCursor(key);
				if(key & KEY_A) placeShip();
				break;
			case setup:
				switchBoards();
				break;
			case play:
				moveCursor(key);
				if(key & KEY_A) checkCell();
				break;
			case end:
				break;
		}
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
		
		glBegin2D();
		switch (getStatus()) {
			case menu:
			case lobby:
			case join:
				drawMenu();
				break;
			case place:
			case setup:
			case play:
				drawBoard(frame % 2);
				break;
			case end:
		}
		glEnd2D();
		glFlush(0);
		swiWaitForVBlank();
		
	}

	return 0;
}


void initSubSprites(void) {
 
	oamInit(&oamSub, SpriteMapping_Bmp_2D_256, false);
 
	int x = 0;
	int y = 0;
 
	int id = 0;

	//set up a 4x3 grid of 64x64 sprites to cover the screen
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
