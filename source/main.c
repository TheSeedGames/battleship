#include <nds.h>
#include <dswifi9.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl2d.h>


#define HALF_WIDTH (SCREEN_WIDTH/2)
#define HALF_HEIGHT (SCREEN_HEIGHT/2)
#define BRAD_PI (1 << 14)

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

#define GRID_SIDE	10
#define GRID_SIZE	100
#define CELL_SIDE	16
#define BOARD_SIDE CELL_SIDE*GRID_SIDE

enum cell_status {
	CELL_UNKNOWN = 0,
	CELL_WATER,
	CELL_SHIP_INTACT,
	CELL_SHIP_HIT,
	CELL_SHIP_DAMAGED,
	CELL_SHIP_SUNKEN
} cell_status;

enum play_status {
	menu,
	lobby,
	join,
	place,
	setup,
	play,
	end
} play_status;

typedef struct match_t {
	uint8_t versus:1;
	uint8_t vertical:1;
	uint8_t gameStatus:3;
} match_t;

typedef struct cell_t {
	uint8_t ship_ID: 4;
	uint8_t status: 3;
} cell_t;

typedef struct ship_t {
	uint8_t size: 3;
	uint8_t id: 4;
	uint8_t destroyed: 1;
} ship_t;



void drawMenu();
void drawBoard(cell_t *board);
void drawCursor(uint8_t pos, uint8_t length, bool vert);
bool placeShip (uint8_t pos, uint8_t size, bool vert, cell_t *board);
uint8_t checkCell(cell_t target);
uint8_t getStatus();
void initSubSprites();

match_t flags;

int main() {
	
	//lower the CPU freq
	if (isDSiMode()) setCpuClock(false);
	
	//setup screen modes
	videoSetMode(MODE_5_3D);
	videoSetModeSub(MODE_5_2D);

	cell_t board_A[GRID_SIZE] = {0};
	cell_t board_B[GRID_SIZE] = {0};
	
	
	initSubSprites();
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
	
	ship_t ships[10];
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

	uint16_t key;		// for key input
	uint8_t frame = 0;
	uint8_t cursor = 0;	
	uint8_t ship = 0;
	flags.gameStatus = place;
	
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
				if(key & KEY_B) flags.versus ++;
				if(key & KEY_A) flags.gameStatus = lobby;
				break;
			case lobby:
				if (flags.versus) flags.gameStatus = setup;
				break;
			case join:
				break;
			case place:
				if(key & KEY_B) flags.vertical ++;
				if(key & KEY_UP)    cursor = (cursor + 90) % 100;
				if(key & KEY_DOWN)  cursor = (cursor + 10) % 100;
				if(key & KEY_LEFT)  cursor = (((cursor % 10) + 9) % 10) + (cursor / 10) * 10;
				if(key & KEY_RIGHT) cursor = (((cursor % 10) + 1) % 10) + (cursor / 10) * 10;
				if(flags.vertical) {
					if(cursor / 10 > 10 - ships[ship].size) while (cursor / 10 > 10 - ships[ship].size)  cursor -= 10;
				} else { 
					if (cursor % 10 > 10 - ships[ship].size) while(cursor % 10 > 10 - ships[ship].size) cursor -= 1; 
				}
				if(key & KEY_A){
					if(placeShip(cursor, ships[ship].size, flags.vertical, board_A)){
						ship++;
						if(ship > 9) {
							flags.gameStatus = setup;
						}
					}
				}
				break;
			case setup:
				for(uint8_t c = 0; c<100; c++) {
					if (board_A[c].status==CELL_UNKNOWN) board_A[c].status=CELL_WATER;
				}
				DC_FlushRange(&board_A,100);
				memcpy(board_B,board_A,100);
				DC_FlushRange(&board_B,100);
				memset(board_A,0,100);
				DC_FlushRange(&board_A,100);
				DC_InvalidateRange(&board_A,100);
				DC_InvalidateRange(&board_B,100);
				flags.gameStatus = play;
				break;
			case play:
				if(key & KEY_UP)    cursor = (cursor + 90) % 100;
				if(key & KEY_DOWN)  cursor = (cursor + 10) % 100;
				if(key & KEY_LEFT)  cursor = (((cursor % 10) + 9) % 10) + (cursor / 10) * 10;
				if(key & KEY_RIGHT) cursor = (((cursor % 10) + 1) % 10) + (cursor / 10) * 10;
				if(key & KEY_A)		board_A[cursor].status = checkCell(board_B[cursor]);
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
				if (frame % 2) {
					drawBoard(board_A);	
					drawCursor(cursor, ships[ship].size, flags.vertical);
				} else {
					drawBoard(board_B);
				}
				break;
			case end:
		}
		glEnd2D();
		glFlush(0);
		swiWaitForVBlank();
		
	}

	return 0;
}

void drawMenu() {
	
}

void drawBoard(cell_t *board){
	//BACKGROUND
	glBoxFilled(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,DARK_GREY);
	//TITLE
	glBoxFilled(4, 4, SCREEN_WIDTH - 4, 24, LIGHT_GREY);
	//BOARD BACKGROUND
	glBoxFilled(4,28,BOARD_SIDE + 4, BOARD_SIDE + 28, WHITE);
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

void drawShips() {
	//SCOREBOARD BACKGROUND
	glBoxFilled(BOARD_SIDE + 8, 28, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 4,LIGHT_GREY);
	//SHIPS
	glBoxFilled(BOARD_SIDE + 18, 32, BOARD_SIDE + 18 + CELL_SIDE * 4, 48, GREEN);
}

void drawCursor(uint8_t pos, uint8_t size, bool vert) {
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

bool placeShip(uint8_t pos, uint8_t size, bool vert, cell_t *board) {
	uint8_t step = vert ? 10 : 1;
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
inline uint8_t checkCell(cell_t target) 
{
	return target.status;
}
inline uint8_t getStatus() {
	return flags.gameStatus;
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
