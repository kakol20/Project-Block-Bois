#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "player.h"

// LEVELS
#include "levels/Stage1Color.h"
#include "levels/Stage1ColorWalls.h"
#include "levels/Stage2ColorCombo.h"
#include "levels/Stage2ColorComboWALL.h"
#include "levels/Stage3NumberCombo.h"
#include "levels/Stage3NumberComboWalls.h"
#include "levels/Stage4.h"
#include "levels/Stage4Walls.h"
#include "levels/Stage5new.h"
#include "levels/Stage5newWalls.h"

#include "levels/Stage6Both.h"
#include "levels/Stage6BothWalls.h"
#include "levels/Stage7Both.h"
#include "levels/Stage7BothWalls.h"
#include "levels/Stage9Both.h"
#include "levels/Stage9Walls.h"
#include "levels/StageTutPnP.h"
#include "levels/StageTutPnPWALLS.h"

// BOXES
#include "boxes/BoxClear.h"
#include "boxes/Box1.h"
#include "boxes/Box2.h"
#include "boxes/Box3.h"
#include "boxes/Box4.h"
#include "boxes/Box5.h"
#include "boxes/Box6.h"
#include "boxes/Box7.h"
#include "boxes/Box8.h"
#include "boxes/Box9.h"

// DROPBOXES
#include "dropbox/DBox1Clear.h"
#include "dropbox/DBox2Clear.h"
#include "dropbox/DBox3Clear.h"
#include "dropbox/DBox4Clear.h"
#include "dropbox/DBox5Clear.h"
#include "dropbox/DBox6Clear.h"
#include "dropbox/DBox7Clear.h"
#include "dropbox/DBox8Clear.h"
#include "dropbox/DBox9Clear.h"
#include "dropbox/DBoxClear.h"

// MISC
#include "other/EndZone.h"
#include "other/MovingBlocks.h"

void draw();
void update();
void nextLevel();

void init();
void initTutorial();
void initStage1();
void initStage2();

void initStage3();
void initStage4();
void initStage5();

void initStage6();
void initStage7();
void initStage9();

void createGrid(const unsigned short *map);
void addBackground(const unsigned short *wallTiles, const unsigned short *wallMap, const unsigned short *floorTiles, const unsigned short *floorMap);
void move(int changeX, int changeY);
bool merge(int boxToMerge);
bool isOdd(int n);
bool correctBox(Box box, Box dropbox);
int collision(int x, int y, bool mergeBoxes);

void attractModeInit();
void attractModeUdate(int step);

// VARTIABLES
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *simplePlayer = &obj_buffer[0];

Box boxes[NUMBER_BOXES]; // boxes
Box dropboxes[NUMBER_DBOXES]; // dropboxes

Coordinates list[39];

Player player;
Sprite end;
Sprite gates[NUMBER_GATES]; 

int screenWidth = 240;
int screenHeight = 160;
int backgroundX = 0;
int backgroundY = 0;

int gameState = 0;
int menuSelection = 0;
int levelSelection = 2;
int xDistance;
int yDistance;
int nextBuffer; // to keep count 
int currBox;
int menuState = 0;

int pScore = 0;
int numStep = 0;
int hScore = 99999;
int currentState = 0;

bool lvl1Played = false;
bool lvl2Played = false;
bool lvl3Played = false;
bool lvl4Played = false;
bool lvl5Played = false;
bool lvl6Played = false;

unsigned short world_grid[32][32];

bool isOdd(int n) {
	if ((n % 2) == 1) {
		return true;
	}
	return false;
}

int collision(int x, int y, bool mergeBoxes) {

	int i;
	for (i = 0; i < NUMBER_BOXES; i++) {
		if ((x == boxes[i].worldX) && (y == boxes[i].worldY)) { // Checks if there is a box on the x,y position
			if (mergeBoxes) {
				if (merge(i)) {
					return -1; // return -1 to say that it can move
				}
			}
		
			return i; // returns the index in the boxes[] array
		}
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		if ((x == gates[i].worldX) && (y == gates[i].worldY)) {
			return NUMBER_BOXES + 1; // returns NUMBER_BOXES + 1 if colliding with a gate
		}
	}
	
	return -1; // returns -1 if nothing was found
}

bool merge(int boxToMerge) {
	// check if the boxes are compatible
	// red = 1, orange = 2, yellow = 3, green = 4, blue = 5, purple = 6 pallette bank
	// if both are odd, they are compatible
	// x + y > 9 not compatible
	
	/* boxes can't merge if their sum value will be greater than 9*/
	/* boxes can merge if they are the same colour*/
	
	if (((isOdd(boxes[boxToMerge].pb) && isOdd(boxes[currBox].pb) && (boxToMerge >= 0)) || (boxes[boxToMerge].pb == boxes[currBox].pb)) && ((boxes[currBox].value + boxes[boxToMerge].value) <= 9)) {
		u32 newColour = boxes[boxToMerge].pb + boxes[currBox].pb;
		
		if (boxes[boxToMerge].pb == boxes[currBox].pb) {
			newColour = boxes[currBox].pb;
		}
		
		else if (newColour != 6) {
			newColour = newColour / 2; // 1 + 5 makes purple, but 1 + 3 does not make orange, we instead half it
		}
		
		boxes[currBox].pb = newColour;
		
		boxes[currBox].value = boxes[currBox].value + boxes[boxToMerge].value;
		
		
		boxes[boxToMerge].worldX = -128; // move to out of the screen
		boxes[boxToMerge].worldY = -128;
		
		
		return true;
	}
	
	return false; // return if they have been merged
}

bool correctBox(Box box, Box dropbox) {
	if ((box.worldX == dropbox.worldX) && (box.worldY == dropbox.worldY) && (box.pb == dropbox.pb) && (box.value == dropbox.value)) {
		return true;
	} else {
		return false;
	}
}

void move(int changeX, int changeY) { // put in parameters how much the position of the player should change - example: moving up move(0, -1)
	// key_poll(); // checks for key inputs

	bool move = false;
		
	if (!world_grid[player.x + changeX][player.y + changeY]) {
		if (key_held(KEY_A)) {		
			currBox = collision(player.x + changeX, player.y + changeY, false); // pushing
			
			if ((currBox >= 0) && !world_grid[player.x + changeX + changeX][player.y + changeY + changeY] && (collision(player.x + changeX + changeX, player.y + changeY + changeY, true) < 0)) {
				
				// example move(0, 1)
				// currBox = (player x position, player y position + 1)
				// box ahea of currBox = (player x position, player y position + 2
				
				if (currBox < NUMBER_BOXES) { // can only push a box
					boxes[currBox].worldX += changeX;
					boxes[currBox].worldY += changeY;	
				}
				
				move = true;
			}
		}
		
		if (key_held(KEY_B)) { // pulling
			currBox = collision(player.x - changeX, player.y - changeY, false);
			
			if ((currBox >= 0) && (collision(player.x + changeX, player.y + changeY, false) < 0)) {
				if (currBox < NUMBER_BOXES) { // can only pull a box
					boxes[currBox].worldX += changeX;
					boxes[currBox].worldY += changeY;
				}
				
				move = true;
			} else if (collision(player.x + changeX, player.y + changeY, false) < 0) {
				move = true;
			}
		} else if (collision(player.x + changeX, player.y + changeY, false) < 0){
			move = true;
		}
	}
	
	if (move) {
		backgroundX += changeX * 8;
		backgroundY += changeY * 8;
		
		player.x += changeX;
		player.y += changeY;
	}
}

void createGrid(const unsigned short *map) {
	int xx, yy;
	for (yy = 0; yy < 32; yy++) {
		for (xx = 0; xx < 32; xx++) {
			world_grid[xx][yy] = map[xx + (yy * 32)]; // collision with the walls
		}
	}
}

void addBackground(const unsigned short *wallTiles, const unsigned short *wallMap, const unsigned short *floorTiles, const unsigned short *floorMap) {
	memcpy(pal_bg_mem, 	StageTutPnPPal, StageTutPnPPalLen /* Should be the same for all levels*/);
	memcpy(&tile_mem[1][0], wallTiles, StageTutPnPTilesLen /* Should be the same for all levels*/);
	memcpy(&se_mem[30][0], wallMap, StageTutPnPMapLen /* Should be the same for all levels*/);
	REG_BG1CNT = BG_CBB(1) | BG_SBB(30) | BG_4BPP | BG_REG_64x64 | BG_PRIO(1);
	
	memcpy(&tile_mem[2][0], floorTiles,StageTutPnPTilesLen /* Should be the same for all levels*/);
	memcpy(&se_mem[25][0], floorMap, StageTutPnPMapLen /* Should be the same for all levels*/);
	REG_BG2CNT = BG_CBB(2) | BG_SBB(25) | BG_4BPP | BG_REG_64x64 | BG_PRIO(1); // Using 64 x 64 despite map being 32 x 32 to avoid the map from being repeated
}

void init() {
	memcpy(pal_bg_mem, StageTutPnPPal, StageTutPnPPalLen /* Should be the same for all levels*/);

	// SPRITES

	memcpy(&tile_mem[4][0 /*1*/], playerTiles, playerTilesLen);
	memcpy(pal_obj_mem, playerPal, playerPalLen);
	player.sprite = &obj_buffer[0];
	player.pb = 0;
	player.tid = 0;
	player.width = 8;
	player.height = 8;

	memcpy(&tile_mem[4][1 /*2*/], BoxClearTiles, BoxClearTilesLen);
	memcpy(pal_obj_mem, BoxClearPal, BoxClearPalLen);
	
	memcpy(&tile_mem[4][2], Box1Tiles, Box1TilesLen);
	memcpy(pal_obj_mem, Box1Pal, Box1PalLen);
	
	memcpy(&tile_mem[4][3], Box2Tiles, Box2TilesLen);
	memcpy(pal_obj_mem, Box2Pal, Box2PalLen);
	
	memcpy(&tile_mem[4][4], Box3Tiles, Box3TilesLen);
	memcpy(pal_obj_mem, Box3Pal, Box3PalLen);
	
	memcpy(&tile_mem[4][5], Box4Tiles, Box4TilesLen);
	memcpy(pal_obj_mem, Box4Pal, Box4PalLen);
	
	memcpy(&tile_mem[4][6], Box5Tiles, Box5TilesLen);
	memcpy(pal_obj_mem, Box5Pal, Box5PalLen);
	
	memcpy(&tile_mem[4][7], Box6Tiles, Box6TilesLen);
	memcpy(pal_obj_mem, Box6Pal, Box6PalLen);
	
	memcpy(&tile_mem[4][8], Box7Tiles, Box7TilesLen);
	memcpy(pal_obj_mem, Box7Pal, Box7PalLen);
	
	memcpy(&tile_mem[4][9], Box8Tiles, Box8TilesLen);
	memcpy(pal_obj_mem, Box8Pal, Box8PalLen);
	
	memcpy(&tile_mem[4][10], Box9Tiles, Box9TilesLen);
	memcpy(pal_obj_mem, Box9Pal, Box9PalLen);
	
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // creates NUMBER_BOXES amount of boxes
		boxes[i].sprite = &obj_buffer[i + 1];
		boxes[i].pb = 1;
		
		boxes[i].value = 0;
		boxes[i].tid = boxes[i].value + 1;;
		
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
		boxes[i].screenX = (boxes[i].worldX * 8) - backgroundX; // sets in position on the screen
		boxes[i].screenY = (boxes[i].worldY * 8) - backgroundY;
		nextBuffer = i + 2;
	}	
	
	memcpy(&tile_mem[4][11], EndZoneTiles, EndZoneTilesLen);
	memcpy(pal_obj_mem, EndZonePal, EndZonePalLen);
	end.sprite = &obj_buffer[nextBuffer];
	end.pb = 14;
	end.tid = 11;
	
	nextBuffer++;
	
	memcpy(&tile_mem[4][12], DBoxclearTiles, DBoxclearTilesLen);
	memcpy(pal_obj_mem, DBoxclearPal, DBoxclearPalLen);
	
	memcpy(&tile_mem[4][13], DBox1clearTiles, DBox1clearTilesLen);
	memcpy(pal_obj_mem, DBox1clearPal, DBox1clearPalLen);
	
	memcpy(&tile_mem[4][14], DBox2clearTiles, DBox2clearTilesLen);
	memcpy(pal_obj_mem, DBox2clearPal, DBox2clearPalLen);
	
	memcpy(&tile_mem[4][15], DBox3clearTiles, DBox3clearTilesLen);
	memcpy(pal_obj_mem, DBox3clearPal, DBox3clearPalLen);
	
	memcpy(&tile_mem[4][16], DBox4clearTiles, DBox4clearTilesLen);
	memcpy(pal_obj_mem, DBox4clearPal, DBox4clearPalLen);
	
	memcpy(&tile_mem[4][17], DBox5clearTiles, DBox5clearTilesLen);
	memcpy(pal_obj_mem, DBox5clearPal, DBox5clearPalLen);
	
	memcpy(&tile_mem[4][18], DBox6clearTiles, DBox6clearTilesLen);
	memcpy(pal_obj_mem, DBox6clearPal, DBox6clearPalLen);
	
	memcpy(&tile_mem[4][19], DBox7clearTiles, DBox7clearTilesLen);
	memcpy(pal_obj_mem, DBox7clearPal, DBox7clearPalLen);
	
	memcpy(&tile_mem[4][20], DBox8clearTiles, DBox8clearTilesLen);
	memcpy(pal_obj_mem, DBox8clearPal, DBox8clearPalLen);
	
	memcpy(&tile_mem[4][21], DBox9clearTiles, DBox9clearTilesLen);
	memcpy(pal_obj_mem, DBox9clearPal, DBox9clearPalLen);
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].sprite = &obj_buffer[nextBuffer];
		dropboxes[i].pb = 1;
		
		dropboxes[i].value = 0;
		dropboxes[i].tid = 12 + dropboxes[i].value;
		
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
		dropboxes[i].screenX = (dropboxes[i].worldX * 8) - backgroundX;
		dropboxes[i].screenY = (dropboxes[i].worldY * 8) - backgroundY;
		
		nextBuffer++;
	}
	
	memcpy(&tile_mem[4][22], MovingBlocksTiles, MovingBlocksTilesLen);
	memcpy(pal_obj_mem, MovingBlocksPal, MovingBlocksPalLen);
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		gates[i].sprite = &obj_buffer[nextBuffer];
		gates[i].pb = 0;
		gates[i].tid = 22;
		gates[i].worldX = -64;
		gates[i].worldY = -64;
		gates[i].screenX = (gates[i].worldX * 8) - backgroundX;
		gates[i].screenY = (gates[i].worldY * 8) - backgroundY;
		
		nextBuffer++;	
	}

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	oam_init(obj_buffer, 128);
}

void initTutorial() { // replace any existing map with the this level
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 4;
	player.y = 5;
	
	boxes[0].pb = 1;
	boxes[0].value = 0;
	boxes[0].worldX = 7;
	boxes[0].worldY = 4;
	
	boxes[1].pb = 5;
	boxes[1].value = 0;
	boxes[1].worldX = 7;
	boxes[1].worldY = 7;
	
	dropboxes[0].pb = 1;
	dropboxes[0].value = 0;
	dropboxes[0].worldX = 13;
	dropboxes[0].worldY = 3;
	
	dropboxes[1].pb = 5;
	dropboxes[1].value = 0;
	dropboxes[1].worldX = 13;
	dropboxes[1].worldY = 8;
	
	gates[0].worldX = 14;
	gates[0].worldY = 5;
	
	gates[1].worldX = 14;
	gates[1].worldY = 6;
	
	gates[2].worldX = 16;
	gates[2].worldY = 6;
	
	gates[3].worldX = 16;
	gates[3].worldY = 5;
	
	end.worldX = 18;
	end.worldY = 6;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background

	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(StageTutPnPWALLSTiles, StageTutPnPWALLSMap, StageTutPnPTiles, StageTutPnPMap);
	
	// WORLD GRID

	createGrid(StageTutPnPWALLSMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

void initStage1() {
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}

	player.x = 3;
	player.y = 6;
	
	boxes[0].pb = 1;
	boxes[0].value = 0;
	boxes[0].worldX = 9;
	boxes[0].worldY = 5;
	
	boxes[1].pb = 1;
	boxes[1].value = 0;
	boxes[1].worldX = 11;
	boxes[1].worldY = 5;
	
	boxes[2].pb = 1;
	boxes[2].value = 0;
	boxes[2].worldX = 17;
	boxes[2].worldY = 5;
	
	boxes[3].pb = 3;
	boxes[3].value = 0;
	boxes[3].worldX = 17;
	boxes[3].worldY = 6;
	
	boxes[4].pb = 5;
	boxes[4].value = 0;
	boxes[4].worldX = 12;
	boxes[4].worldY = 9;
	
	dropboxes[0].pb = 5;
	dropboxes[0].value = 0;
	dropboxes[0].worldX = 17;
	dropboxes[0].worldY = 4;
	
	gates[0].worldX = 14;
	gates[0].worldY = 11;
	
	gates[1].worldX = 15;
	gates[1].worldY = 11;
	
	gates[2].worldX = 14;
	gates[2].worldY = 12;
	
	gates[3].worldX = 15;
	gates[3].worldY = 12;
	
	end.worldX = 15;
	end.worldY = 15;
	

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background

	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage1ColorWallsTiles, Stage1ColorWallsMap, Stage1ColorTiles, Stage1ColorMap);
	
	// WORLD GRID

	createGrid(Stage1ColorWallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

void initStage2() {
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 2;
	player.y = 4;
	//First Room
	boxes[0].pb = 1;
	boxes[0].value = 0;
	boxes[0].worldX = 6;
	boxes[0].worldY = 4;
	
	boxes[1].pb = 3;
	boxes[1].value = 0;
	boxes[1].worldX = 9;
	boxes[1].worldY = 4;
	
	dropboxes[0].pb = 2;
	dropboxes[0].value = 0;
	dropboxes[0].worldX = 12;
	dropboxes[0].worldY = 4;
	//1st Gates
	gates[0].worldX = 17;
	gates[0].worldY = 4;
	
	gates[1].worldX = 18;
	gates[1].worldY = 5;
	//Second Room
	dropboxes[1].pb = 4;
	dropboxes[1].value = 0;
	dropboxes[1].worldX = 12;
	dropboxes[1].worldY = 8;
	
	dropboxes[2].pb = 2;
	dropboxes[2].value = 0;
	dropboxes[2].worldX = 10;
	dropboxes[2].worldY = 8;
	
	dropboxes[3].pb = 6;
	dropboxes[3].value = 0;
	dropboxes[3].worldX = 8;
	dropboxes[3].worldY = 8;
	
	boxes[2].pb = 5;
	boxes[2].value = 0;
	boxes[2].worldX = 13;
	boxes[2].worldY = 12;
	
	boxes[3].pb = 3;
	boxes[3].value = 0;
	boxes[3].worldX = 12;
	boxes[3].worldY = 11;
	
	boxes[4].pb = 3;
	boxes[4].value = 0;
	boxes[4].worldX = 11;
	boxes[4].worldY = 14;
	
	boxes[5].pb = 5;
	boxes[5].value = 0;
	boxes[5].worldX = 9;
	boxes[5].worldY = 14;
	
	boxes[6].pb = 1;
	boxes[6].value = 0;
	boxes[6].worldX = 8;
	boxes[6].worldY = 9;
	
	boxes[7].pb = 1;
	boxes[7].value = 0;
	boxes[7].worldX = 8;
	boxes[7].worldY = 10;
	
	//2nd room Gates
	gates[2].worldX = 9;
	gates[2].worldY = 13;
	
	gates[3].worldX = 11;
	gates[3].worldY = 13;
	//last gate
	gates[4].worldX = 6;
	gates[4].worldY = 11;
	
	end.worldX = 6;
	end.worldY = 15;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage2ColorComboWALLTiles, Stage2ColorComboWALLMap, Stage2ColorComboTiles, Stage2ColorComboMap);
	
	// WORLD GRID

	createGrid(Stage2ColorComboWALLMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
	
}
void initStage3() {
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 2;
	player.y = 4;
	
	//First Room
	boxes[0].pb = 1;
	boxes[0].value = 2;
	boxes[0].worldX = 6;
	boxes[0].worldY = 4;
	
	boxes[1].pb = 1;
	boxes[1].value = 2;
	boxes[1].worldX = 9;
	boxes[1].worldY = 4;
	
	dropboxes[0].pb = 1;
	dropboxes[0].value = 4;
	dropboxes[0].worldX = 13;
	dropboxes[0].worldY = 4;
	
	//1st Gate
	gates[0].worldX = 17;
	gates[0].worldY = 4;
	
	//Corridor Rooms left
	boxes[2].pb = 3;
	boxes[2].value = 2;
	boxes[2].worldX = 21;
	boxes[2].worldY = 11;
	
	boxes[3].pb = 3;
	boxes[3].value = 2;
	boxes[3].worldX = 16;
	boxes[3].worldY = 12;
	
	dropboxes[1].pb = 3;
	dropboxes[1].value = 4;
	dropboxes[1].worldX = 16;
	dropboxes[1].worldY = 11;
	
	//Corridor Rooms Right
	boxes[4].pb = 5;
	boxes[4].value = 1;
	boxes[4].worldX = 27;
	boxes[4].worldY = 12;
	
	boxes[5].pb = 5;
	boxes[5].value = 5;
	boxes[5].worldX = 25;
	boxes[5].worldY = 11;
	
	dropboxes[2].pb = 5;
	dropboxes[2].value = 6;
	dropboxes[2].worldX = 28;
	dropboxes[2].worldY = 11;
	
	//2nd gates
	
	gates[1].worldX = 24;
	gates[1].worldY = 13;
	
	gates[2].worldX = 20;
	gates[2].worldY = 13;
	
	end.worldX = 22;
	end.worldY = 19;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage3NumberComboWallsTiles, Stage3NumberComboWallsMap, Stage3NumberComboTiles, Stage3NumberComboMap);
	
	// WORLD GRID

	createGrid(Stage3NumberComboWallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

void initStage4() {
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 12;
	player.y = 6;
	
	//room1
	
	boxes[0].pb = 1;
	boxes[0].value = 4;
	boxes[0].worldX = 13;
	boxes[0].worldY = 2;
	
	boxes[1].pb = 5;
	boxes[1].value = 3;
	boxes[1].worldX = 13;
	boxes[1].worldY = 9;
	
	dropboxes[0].pb = 6;
	dropboxes[0].value = 7;
	dropboxes[0].worldX = 6;
	dropboxes[0].worldY = 6;
	
	end.worldX = 3;
	end.worldY = 13;
	
	//Gates 1
	
	gates[0].worldX = 16;
	gates[0].worldY = 5;
	
	gates[1].worldX = 17;
	gates[1].worldY = 5;
	
	gates[2].worldX = 16;
	gates[2].worldY = 6;
	
	gates[3].worldX = 17;
	gates[3].worldY = 6;
	
	//Gates 2
	
	gates[4].worldX = 19;
	gates[4].worldY = 5;
	
	gates[5].worldX = 20;
	gates[5].worldY = 5;
	
	gates[6].worldX = 19;
	gates[6].worldY = 6;
	
	gates[7].worldX = 20;
	gates[7].worldY = 6;
	
	//room 2
	
	boxes[2].pb = 1;
	boxes[2].value = 6;
	boxes[2].worldX = 24;
	boxes[2].worldY = 8;
	
	boxes[3].pb = 3;
	boxes[3].value = 3;
	boxes[3].worldX = 19;
	boxes[3].worldY = 11;
	
	boxes[4].pb = 5;
	boxes[4].value = 3;
	boxes[4].worldX = 22;
	boxes[4].worldY = 11;
	
	boxes[5].pb = 5;
	boxes[5].value = 3;
	boxes[5].worldX = 22;
	boxes[5].worldY = 12;
	
	boxes[6].pb = 5;
	boxes[6].value = 3;
	boxes[6].worldX = 19;
	boxes[6].worldY = 13;
	
	dropboxes[1].pb = 2;
	dropboxes[1].value = 9;
	dropboxes[1].worldX = 19;
	dropboxes[1].worldY = 14;
	
	gates[8].worldX = 17;
	gates[8].worldY = 12;
	
	gates[9].worldX = 17;
	gates[9].worldY = 13;
	
	gates[10].worldX = 16;
	gates[10].worldY = 12;
	
	gates[11].worldX = 16;
	gates[11].worldY = 13;
	
	gates[12].worldX = 15;
	gates[12].worldY = 12;
	
	gates[13].worldX = 15;
	gates[13].worldY = 13;
	
	//Lst Room
	
	boxes[7].pb = 3;
	boxes[7].value = 3;
	boxes[7].worldX = 12;
	boxes[7].worldY = 11;
	
	boxes[8].pb = 5;
	boxes[8].value = 3;
	boxes[8].worldX = 12;
	boxes[8].worldY = 15;
	
	dropboxes[2].pb = 4;
	dropboxes[2].value = 6;
	dropboxes[2].worldX = 7;
	dropboxes[2].worldY = 13;
	
	gates[14].worldX = 5;
	gates[14].worldY = 13;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage4WallsTiles, Stage4WallsMap, Stage4Tiles, Stage4Map);
	
	// WORLD GRID

	createGrid(Stage4WallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

void initStage5() {
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 3;
	player.y = 3;
	//R1
	boxes[0].worldX = 3;
	boxes[0].worldY = 4;
	boxes[0].pb = 3;
	boxes[0].value = 0;
	
	boxes[1].worldX = 3;
	boxes[1].worldY = 6;
	boxes[1].pb = 1;
	boxes[1].value = 0;
	
	dropboxes[0].worldX = 4;
	dropboxes[0].worldY = 3;
	dropboxes[0].pb = 2;
	dropboxes[0].value = 0;
	
	gates[0].worldX = 7;
	gates[0].worldY = 4;
	
	gates[1].worldX = 7;
	gates[1].worldY = 5;
	//R2
	boxes[3].worldX = 9;
	boxes[3].worldY = 3;
	boxes[3].pb = 3;
	boxes[3].value = 1;
	
	boxes[4].worldX = 10;
	boxes[4].worldY = 4;
	boxes[4].pb = 3;
	boxes[4].value = 3;
	
	boxes[5].worldX = 9;
	boxes[5].worldY = 6;
	boxes[5].pb = 5;
	boxes[5].value = 6;
	
	dropboxes[1].worldX = 10;
	dropboxes[1].worldY = 3;
	dropboxes[1].pb = 3;
	dropboxes[1].value = 4;
	
	gates[2].worldX = 13;
	gates[2].worldY = 4;
	
	gates[3].worldX = 13;
	gates[3].worldY = 5;
	//R3
	boxes[6].worldX = 16;
	boxes[6].worldY = 3;
	boxes[6].pb = 3;
	boxes[6].value = 2;
	
	dropboxes[2].worldX = 17;
	dropboxes[2].worldY = 6;
	dropboxes[2].pb = 4;
	dropboxes[2].value = 8;
	
	gates[4].worldX = 19;
	gates[4].worldY = 4;
	
	gates[5].worldX = 19;
	gates[5].worldY = 5;
	//R4
	boxes[7].worldX = 24;
	boxes[7].worldY = 4;
	boxes[7].pb = 3;
	boxes[7].value = 0;
	
	boxes[8].worldX = 24;
	boxes[8].worldY = 5;
	boxes[8].pb = 3;
	boxes[8].value = 0;
	
	//AAa
	boxes[9].worldX = 22;
	boxes[9].worldY = 12;
	boxes[9].pb = 1;
	boxes[9].value = 0;
	
	boxes[10].worldX = 19;
	boxes[10].worldY = 12;
	boxes[10].pb = 3;
	boxes[10].value = 0;
	
	boxes[11].worldX = 18;
	boxes[11].worldY = 12;
	boxes[11].pb = 1;
	boxes[11].value = 0;
	
	boxes[12].worldX = 15;
	boxes[12].worldY = 12;
	boxes[12].pb = 1;
	boxes[12].value = 0;
	
	boxes[13].worldX = 12;
	boxes[13].worldY = 13;
	boxes[13].pb = 1;
	boxes[13].value = 0;
	
	end.worldX = 17;
	end.worldY = 17;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage5newWallsTiles, Stage5newWallsMap, Stage5newTiles, Stage5newMap);
	
	// WORLD GRID

	createGrid(Stage5newWallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

}
void initStage6(){
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 2;
	player.y = 7;
	//Up
	boxes[0].worldX = 6;
	boxes[0].worldY = 4;
	boxes[0].pb = 1;
	boxes[0].value = 3;
	
	boxes[1].worldX = 7;
	boxes[1].worldY = 6;
	boxes[1].pb = 3;
	boxes[1].value = 4;
	
	dropboxes[0].worldX = 10;
	dropboxes[0].worldY = 6;
	dropboxes[0].pb = 2;
	dropboxes[0].value = 7;
	
	gates[0].worldX = 10;
	gates[0].worldY = 3;
	
	gates[1].worldX = 9;
	gates[1].worldY = 4;
	//Down
	boxes[2].worldX = 6;
	boxes[2].worldY = 10;
	boxes[2].pb = 3;
	boxes[2].value = 2;
	
	boxes[3].worldX = 10;
	boxes[3].worldY = 8;
	boxes[3].pb = 5;
	boxes[3].value = 3;
	
	dropboxes[1].worldX = 12;
	dropboxes[1].worldY = 1;
	dropboxes[1].pb = 4;
	dropboxes[1].value = 5;
	
	gates[2].worldX = 10;
	gates[2].worldY = 11;
	
	gates[3].worldX = 9;
	gates[3].worldY = 10;
	//Right
	boxes[4].worldX = 12;
	boxes[4].worldY = 8;
	boxes[4].pb = 5;
	boxes[4].value = 3;
	
	boxes[5].worldX = 12;
	boxes[5].worldY = 6;
	boxes[5].pb = 1;
	boxes[5].value = 5;
	
	boxes[6].worldX = 16;
	boxes[6].worldY = 7;
	boxes[6].pb = 1;
	boxes[6].value = 0;
	
	dropboxes[2].worldX = 12;
	dropboxes[2].worldY = 13;
	dropboxes[2].pb = 6;
	dropboxes[2].value = 8;
	
	gates[4].worldX = 17;
	gates[4].worldY = 7;
	
	gates[5].worldX = 16;
	gates[5].worldY = 6;
	
	gates[6].worldX = 16;
	gates[6].worldY = 8;
	//Last Room
	
	boxes[7].worldX = 20;
	boxes[7].worldY = 15;
	boxes[7].pb = 3;
	boxes[7].value = 1;
	
	boxes[8].worldX = 20;
	boxes[8].worldY = 16;
	boxes[8].pb = 3;
	boxes[8].value = 1;
	
	boxes[9].worldX = 16;
	boxes[9].worldY = 15;
	boxes[9].pb = 1;
	boxes[9].value = 3;
	
	boxes[10].worldX = 17;
	boxes[10].worldY = 15;
	boxes[10].pb = 5;
	boxes[10].value = 3;
	
	boxes[11].worldX = 15;
	boxes[11].worldY = 20;
	boxes[11].pb = 5;
	boxes[11].value = 6;
	
	boxes[12].worldX = 14;
	boxes[12].worldY = 20;
	boxes[12].pb = 1;
	boxes[12].value = 3;
	
	dropboxes[3].worldX = 18;
	dropboxes[3].worldY = 21;
	dropboxes[3].pb = 2;
	dropboxes[3].value = 4;
	
	dropboxes[4].worldX = 13;
	dropboxes[4].worldY = 15;
	dropboxes[4].pb = 6;
	dropboxes[4].value = 9;
	
	gates[7].worldX = 17;
	gates[7].worldY = 19;
	
	gates[8].worldX = 15;
	gates[8].worldY = 17;
	
	gates[9].worldX = 11;
	gates[9].worldY = 19;
	
	end.worldX = 11;
	end.worldY = 21;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage6BothWallsTiles, Stage6BothWallsMap, Stage6BothTiles, Stage6BothMap);
	
	// WORLD GRID

	createGrid(Stage6BothWallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}
/*void initStage7(){
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 3;
	player.y = 3;
	
	end.worldX = 16;
	end.worldY = 16;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage7BothWallsTiles, Stage7BothWallsMap, Stage7BothTiles, Stage7BothMap);
	
	// WORLD GRID

	createGrid(Stage7BothWallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}
void initStage9(){
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}
	
	player.x = 3;
	player.y = 3;
	
	end.worldX = 16;
	end.worldY = 16;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage9WallsTiles, Stage9WallsMap, Stage9BothTiles, Stage9BothMap);
	
	// WORLD GRID

	createGrid(Stage9WallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}*/


void update() {
	// INPUT 

	key_poll(); // checks for key inputs
	
	int changeX = 0;
	int changeY = 0;

	if(key_hit(KEY_SELECT) && (gameState != 0))
	{
	
		currentState = gameState;
	
		gameState = 10;
		
	}
	
	if (key_hit(KEY_UP) && (gameState != 0)) {
		changeY = -1;// parameters are how much should the player position change
		numStep++;
	}
	
	if (key_hit(KEY_RIGHT) && (gameState != 0)) {
		changeX = 1;
		numStep++;
	}
	
	if (key_hit(KEY_DOWN) && (gameState != 0)) {
		changeY = 1;
		numStep++;
	}
	
	if (key_hit(KEY_LEFT) && (gameState != 0)) {
		changeX = -1;
		numStep++;
	}
	
	move(changeX, changeY);
	
	// MOVING TO ANOTHER MAP

	if ((end.worldX == player.x) && (end.worldY == player.y)) { // tempory movement between maps - testing if the player walks over the end, then it will go to another map
		switch (gameState) {
			case 1: // tutorial - gameState 0 is the main menu
				gameState = 2;
				initStage1();
				
				lvl1Played = true;
				
				break;
			case 2: // level 1
				gameState = 3;
				initStage2();
				
				lvl2Played = true;
				
				break;
			case 3: // level 2
				gameState = 4;
				initStage3();
				
				lvl3Played = true;
				
				break;
			case 4: //level 3
				gameState = 5;
				initStage4();
				
				lvl4Played = true;
				
				break;
			case 5: //level 4
				gameState = 6;
				initStage5();
				
				lvl5Played = true;
				
				break;
			case 6: //level 5
				gameState = 7;
				initStage6();
				
				lvl6Played = true;
				
				break;
			case 7: //level 6
				if (hScore > numStep) {
					hScore = numStep;
				}
				
				gameState = 0;
				menuState = 0;
				numStep = 0;
				attractModeInit();
				
				// END OF GAME
				
				break;
		}
	}

	// OPENING GATES
	switch (gameState) {
		case 0: // ATTRACT MODE
			if (correctBox(boxes[1], dropboxes[1])) {
				gates[2].worldX = 16;
				gates[2].worldY = 4;
				
				gates[3].worldX = 16;
				gates[3].worldY = 7;
			} else {
				gates[2].worldX = 16;
				gates[2].worldY = 5;
				
				gates[3].worldX = 16;
				gates[3].worldY = 6;
			}
			
			if (correctBox(boxes[0], dropboxes[0])) {
				gates[0].worldX = 14;
				gates[0].worldY = 4;
				
			gates[1].worldX = 14;
				gates[1].worldY = 7;
			} else {
				gates[0].worldX = 14;
				gates[0].worldY = 5;
				
				gates[1].worldX = 14;
				gates[1].worldY = 6;
			}
			break;
	
		case 1: // TUTORIAL
		
			// checking if the correct box has been placed on the dropbox
			if (correctBox(boxes[1], dropboxes[1])) {
				gates[2].worldX = 16;
				gates[2].worldY = 4;
				
				gates[3].worldX = 16;
				gates[3].worldY = 7;
			} else {
				gates[2].worldX = 16;
				gates[2].worldY = 5;
				
				gates[3].worldX = 16;
				gates[3].worldY = 6;
			}
			
			if (correctBox(boxes[0], dropboxes[0])) {
				gates[0].worldX = 14;
				gates[0].worldY = 4;
				
			gates[1].worldX = 14;
				gates[1].worldY = 7;
			} else {
				gates[0].worldX = 14;
				gates[0].worldY = 5;
				
				gates[1].worldX = 14;
				gates[1].worldY = 6;
			}
		
			break;
			
		case 2: // LEVEL 1
			if (correctBox(boxes[4], dropboxes[0])) {
				gates[0].worldX = 13;
				gates[0].worldY = 11;
				
				gates[1].worldX = 16;
				gates[1].worldY = 11;
				
				gates[2].worldX = 13;
				gates[2].worldY = 12;
				
				gates[3].worldX = 16;
				gates[3].worldY = 12;
			} else {
				gates[0].worldX = 14;
				gates[0].worldY = 11;
				
				gates[1].worldX = 15;
				gates[1].worldY = 11;
				
				gates[2].worldX = 14;
				gates[2].worldY = 12;
				
				gates[3].worldX = 15;
				gates[3].worldY = 12;
			}
			
			break;
			
		case 3: // LEVEL 2
			if (correctBox(boxes[0], dropboxes[0]) || correctBox(boxes[1], dropboxes[0])) { // all possible boxes must be checked
				// player can push box0 into box1 - box1 gets deleted and box0 becomes the new box
				// same thing happens for pushing box1 into box0
			
				gates[0].worldX = 17;
				gates[0].worldY = 3;
				
				gates[1].worldX = 19;
				gates[1].worldY = 5;
			} else {
				gates[0].worldX = 17;
				gates[0].worldY = 4;
				
				gates[1].worldX = 18;
				gates[1].worldY = 5;
			}
			
			if (correctBox(boxes[3], dropboxes[1]) || correctBox(boxes[2], dropboxes[1])) {
				gates[5].worldX = 11;
				gates[5].worldY = 9;
				
				gates[2].worldX = 12;
				gates[2].worldY = 13;
			} else {
				gates[5].worldX = 10;
				gates[5].worldY = 9;
				
				gates[2].worldX = 11;
				gates[2].worldY = 13;
			}
			
			if (correctBox(boxes[5], dropboxes[2]) || correctBox(boxes[7], dropboxes[2]) || correctBox(boxes[4], dropboxes[2])) {
				gates[3].worldX = 10 ;
				gates[3].worldY = 13;
			} else {
				gates[3].worldX = 9;
				gates[3].worldY = 13;
			}
			
			if (correctBox(boxes[7], dropboxes[3]) || correctBox(boxes[5], dropboxes[3]) || correctBox(boxes[6], dropboxes[3])) {
				gates[4].worldX = 7;
				gates[4].worldY = 11;
			} else {
				gates[4].worldX = 6;
				gates[4].worldY = 11;
			}
			
			break;
			
		case 4: // LEVEL 3
			if (correctBox(boxes[0], dropboxes[0]) || correctBox(boxes[1], dropboxes[0])) {
				gates[0].worldX = 17;
				gates[0].worldY = 3;
			} else {
				gates[0].worldX = 17;
				gates[0].worldY = 4;
			}
			
			if (correctBox(boxes[2], dropboxes[1]) || correctBox(boxes[3], dropboxes[1])) {
				gates[1].worldX = 23;
				gates[1].worldY = 11;
			} else {
				gates[1].worldX = 24;
				gates[1].worldY = 11;
			}
			
			if (correctBox(boxes[5], dropboxes[2]) || correctBox(boxes[4], dropboxes[2])) {
				gates[2].worldX = 23;
				gates[2].worldY = 13;
				
				gates[3].worldX = 21;
				gates[3].worldY = 13;
			} else {
				gates[2].worldX = 24;
				gates[2].worldY = 13;
				
				gates[3].worldX = 20;
				gates[3].worldY = 13;
			}
			
			/*if (correctBox(boxes[5], dropboxes[3]) || correctBox(boxes[6], dropboxes[3])) {
				
				gates[4].worldX = 5;
				gates[4].worldY = 11;
				
			} else {
			
				gates[4].worldX = 6;
				gates[4].worldY = 11;
				
			}*/
			
			break;
		
		case 5: // LEVEL 4
			if (correctBox(boxes[0], dropboxes[0]) || correctBox(boxes[1], dropboxes[0])) {
				gates[0].worldX = 16;
				gates[0].worldY = 4;
				
				gates[1].worldX = 17;
				gates[1].worldY = 4;
				
				gates[2].worldX = 16;
				gates[2].worldY = 7;
				
				gates[3].worldX = 17;
				gates[3].worldY = 7;
				
				gates[4].worldX = 19;
				gates[4].worldY = 4;
				
				gates[5].worldX = 20;
				gates[5].worldY = 4;
				
				gates[6].worldX = 19;
				gates[6].worldY = 7;
				
				gates[7].worldX = 20;
				gates[7].worldY = 7;
			} else {
				gates[0].worldX = 16;
				gates[0].worldY = 5;
				
				gates[1].worldX = 17;
				gates[1].worldY = 5;
				
				gates[2].worldX = 16;
				gates[2].worldY = 6;
				
				gates[3].worldX = 17;
				gates[3].worldY = 6;
				
				gates[4].worldX = 19;
				gates[4].worldY = 5;
				
				gates[5].worldX = 20;
				gates[5].worldY = 5;
				
				gates[6].worldX = 19;
				gates[6].worldY = 6;
				
				gates[7].worldX = 20;
				gates[7].worldY = 6;
			}
			if (correctBox(boxes[2], dropboxes[1]) || correctBox(boxes[3], dropboxes[1])) {
				gates[8].worldX = 17;
				gates[8].worldY = 11;
				
				gates[9].worldX = 17;
				gates[9].worldY = 14;
				
				gates[10].worldX = 16;
				gates[10].worldY = 11;
				
				gates[11].worldX = 16;
				gates[11].worldY = 14;
				
				gates[12].worldX = 15;
				gates[12].worldY = 11;
				
				gates[13].worldX = 15;
				gates[13].worldY = 14;
			} else {
				gates[8].worldX = 17;
				gates[8].worldY = 12;
				
				gates[9].worldX = 17;
				gates[9].worldY = 13;
				
				gates[10].worldX = 16;
				gates[10].worldY = 12;
				
				gates[11].worldX = 16;
				gates[11].worldY = 13;
				
				gates[12].worldX = 15;
				gates[12].worldY = 12;
				
				gates[13].worldX = 15;
				gates[13].worldY = 13;
			}
			if (correctBox(boxes[7], dropboxes[2]) || correctBox(boxes[8], dropboxes[2])) {
				gates[14].worldX = 5;
				gates[14].worldY = 14;
			} else {
				gates[14].worldX = 5;
				gates[14].worldY = 13;
			}
			
			break;
		
		case 6: // LEVEL 5
			if (correctBox(boxes[0], dropboxes[0]) || correctBox(boxes[1], dropboxes[0])) {
				gates[0].worldX = 7;
				gates[0].worldY = 3;
				
				gates[1].worldX = 7;
				gates[1].worldY = 6;
			} else {
				gates[0].worldX = 7;
				gates[0].worldY = 4;
				
				gates[1].worldX = 7;
				gates[1].worldY = 5;
			}
				if (correctBox(boxes[3], dropboxes[1]) || correctBox(boxes[4], dropboxes[1])) {
				gates[2].worldX = 13;
				gates[2].worldY = 3;
				
				gates[3].worldX = 13;
				gates[3].worldY = 6;
			} else {
				gates[2].worldX = 13;
				gates[2].worldY = 4;
				
				gates[3].worldX = 13;
				gates[3].worldY = 5;
			}
				if (correctBox(boxes[5], dropboxes[2]) || correctBox(boxes[6], dropboxes[2])) {
				gates[4].worldX = 19;
				gates[4].worldY = 3;
				
				gates[5].worldX = 19;
				gates[5].worldY = 6;
			} else {
				gates[4].worldX = 19;
				gates[4].worldY = 4;
				
				gates[5].worldX = 19;
				gates[5].worldY = 5;
			}
			break;
	
		case 7: // LEVEL 6
			if (correctBox(boxes[0], dropboxes[0]) || correctBox(boxes[1], dropboxes[0])) {
				gates[0].worldX = 9;
				gates[0].worldY = 3;
				
			} else {
				gates[0].worldX = 10;
				gates[0].worldY = 3;

			}
				if (correctBox(boxes[2], dropboxes[1]) || correctBox(boxes[3], dropboxes[1])) {
				gates[3].worldX = 8;
				gates[3].worldY = 10;
				
			} else {
				gates[3].worldX = 9;
				gates[3].worldY = 10;

			}
			if (correctBox(boxes[4], dropboxes[2]) || correctBox(boxes[5], dropboxes[2])) {
				gates[5].worldX = 16;
				gates[5].worldY = 5;
				
				gates[6].worldX = 16;
				gates[6].worldY = 9;
				
			} else {
				gates[5].worldX = 16;
				gates[5].worldY = 6;
				
				gates[6].worldX = 16;
				gates[6].worldY = 8;

			}
			if (correctBox(boxes[8], dropboxes[3]) || correctBox(boxes[7], dropboxes[3]) || correctBox(boxes[9], dropboxes[3])) {
				gates[7].worldX = 17;
				gates[7].worldY = 20;
				
				gates[8].worldX = 15;
				gates[8].worldY = 16;
				
			} else {
				gates[7].worldX = 17;
				gates[7].worldY = 19;
				
				gates[8].worldX = 15;
				gates[8].worldY = 17;

			}
			if (correctBox(boxes[11], dropboxes[4]) || correctBox(boxes[12], dropboxes[4])) {
				gates[9].worldX = 10;
				gates[9].worldY = 19;
				
			} else {
				gates[9].worldX = 11;
				gates[9].worldY = 19;

			}
			break;
		
	}
}

void draw() {	
	int x = (screenWidth - player.width) / 2; // player sprite
	int y = (screenHeight / 2) - player.height;
	obj_set_attr(player.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(player.pb) | player.tid | ATTR2_PRIO(1));
	obj_set_pos(player.sprite, x, y); // puts the player on the screen

	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // goes through boxes[] array and draws it onto the screen
		boxes[i].tid = boxes[i].value + 1;
	
		obj_set_attr(boxes[i].sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(boxes[i].pb) | boxes[i].tid | ATTR2_PRIO(1));

		xDistance = abs(boxes[i].worldX - player.x);
		yDistance = abs(boxes[i].worldY - player.y);

		if ((xDistance > 16) || (yDistance > 16)) { // the box should not be on the screen if it's position is 16 away from the player's position
			boxes[i].screenX = -8;
			boxes[i].screenY = -8; // put it outside the screen
		} else {
			boxes[i].screenX = (boxes[i].worldX * 8) - backgroundX;
			boxes[i].screenY = (boxes[i].worldY * 8) - backgroundY;
		}

		obj_set_pos(boxes[i].sprite, boxes[i].screenX, boxes[i].screenY);
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].tid = dropboxes[i].value + 12;
		
		obj_set_attr(dropboxes[i].sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(dropboxes[i].pb) | dropboxes[i].tid | ATTR2_PRIO(1));
		
		xDistance = abs(dropboxes[i].worldX - player.x);
		yDistance = abs(dropboxes[i].worldY - player.y);
		
		if ((xDistance > 16) || (yDistance > 16)) {
			dropboxes[i].screenX = -8;
			dropboxes[i].screenY = -8;
		} else {
			dropboxes[i].screenX = (dropboxes[i].worldX * 8) - backgroundX;
			dropboxes[i].screenY = (dropboxes[i].worldY * 8) - backgroundY;
		}
		
		obj_set_pos(dropboxes[i].sprite, dropboxes[i].screenX, dropboxes[i].screenY);
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		obj_set_attr(gates[i].sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(gates[i].pb) | gates[i].tid | ATTR2_PRIO(1));
		
		xDistance = abs(gates[i].worldX - player.x);
		yDistance = abs(gates[i].worldY - player.y);
		
		if ((xDistance > 16) || (yDistance > 16)) {
			gates[i].screenX = -8;
			gates[i].screenY = -8;
		} else {
			gates[i].screenX = (gates[i].worldX * 8) - backgroundX;
			gates[i].screenY = (gates[i].worldY * 8) - backgroundY;
		}
		
		obj_set_pos(gates[i].sprite, gates[i].screenX, gates[i].screenY);
	}
	
	xDistance = abs(end.worldX - player.x);
	yDistance = abs(end.worldY - player.y);
	obj_set_attr(end.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(end.pb) | end.tid | ATTR2_PRIO(1));
	if ((xDistance > 16) || (yDistance > 16)) {
		end.screenX = -8;
		end.screenY = -8;
	} else {
		end.screenX = (end.worldX * 8) - backgroundX;
		end.screenY = (end.worldY * 8) - backgroundY;
	}
	obj_set_pos(end.sprite, end.screenX, end.screenY);

	oam_copy(oam_mem, obj_buffer, 128);

	// BACKGROUND

	REG_BG1HOFS = backgroundX;
	REG_BG1VOFS = backgroundY;
	
	REG_BG2HOFS = backgroundX;
	REG_BG2VOFS = backgroundY;
}

void attractModeInit() {
	// AN ARRAY OF COORDINATES IN ORDER OF MOVEMENT
	list[0].x = 4;
	list[0].y = 5;
	
	list[1].x = 5;
	list[1].y = 5;
	
	list[2].x = 6;
	list[2].y = 5;
	
	list[3].x = 7;
	list[3].y = 5;
	
	list[4].x = 7; // PUSH BOX UP
	list[4].y = 4;
	
	list[5].x = 6;
	list[5].y = 4;
	
	list[6].x = 6;
	list[6].y = 3;
	
	list[7].x = 7; // PUSH BOX RIGHT
	list[7].y = 3;
	
	list[8].x = 8; // PUSH BOX RIGHT
	list[8].y = 3;
	
	list[9].x = 9; // PUSH BOX RIGHT
	list[9].y = 3;
	
	list[10].x = 10; // PUSH BOX RIGHT
	list[10].y = 3;
	
	list[11].x = 11; // PUSH BOX RIGHT
	list[11].y = 3;
	
	list[12].x = 12; // PLACE BOX ON DROPBOX - OPENS FIRST GATES
	list[12].y = 3;
	
	list[13].x = 11;
	list[13].y = 3;
	
	list[14].x = 10;
	list[14].y = 3;
	
	list[15].x = 9;
	list[15].y = 3;
	
	list[16].x = 8;
	list[16].y = 3;
	
	list[17].x = 7;
	list[17].y = 3;
	
	list[18].x = 7;
	list[18].y = 4;
	
	list[19].x = 7;
	list[19].y = 5;
	
	list[20].x = 7;
	list[20].y = 6;
	
	list[21].x = 7; // PUSH BOX DOWN
	list[21].y = 7;
	
	list[22].x = 6;
	list[22].y = 7;
	
	list[23].x = 6;
	list[23].y = 8;
	
	list[24].x = 7; // PUSH BOX RIGHT
	list[24].y = 8;
	
	list[25].x = 8; // PUSH BOX RIGHT
	list[25].y = 8;
	
	list[26].x = 9; // PUSH BOX RIGHT
	list[26].y = 8;
	
	list[27].x = 10; // PUSH BOX RIGHT
	list[27].y = 8;
	
	list[28].x = 11; // PUSH BOX RIGHT
	list[28].y = 8;
	
	list[29].x = 12; // PLACE BOX ON DROPBOX - OPENS SECOND GATES
	list[29].y = 8;
	
	list[30].x = 11;
	list[30].y = 8;
	
	list[31].x = 11;
	list[31].y = 7;
	
	list[32].x = 11;
	list[32].y = 6;
	
	list[33].x = 12;
	list[33].y = 6;
	
	list[34].x = 13;
	list[34].y = 6;
	
	list[35].x = 14;
	list[35].y = 6;
	
	list[36].x = 15;
	list[36].y = 6;
	
	list[37].x = 16;
	list[37].y = 6;
	
	list[38].x = 17;
	list[38].y = 6;
	
	list[38].x = 18; // END
	list[38].y = 6;
	
	// ADDING SPRITES AND BACKGROUND
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_DBOXES; i++) {
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	for (i = 0; i < NUMBER_GATES; i++) {
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	}

	
	player.x = 4;
	player.y = 5;
	
	boxes[0].pb = 1;
	boxes[0].value = 0;
	boxes[0].worldX = 7;
	boxes[0].worldY = 4;
	
	boxes[1].pb = 5;
	boxes[1].value = 0;
	boxes[1].worldX = 7;
	boxes[1].worldY = 7;
	
	dropboxes[0].pb = 1;
	dropboxes[0].value = 0;
	dropboxes[0].worldX = 13;
	dropboxes[0].worldY = 3;
	
	dropboxes[1].pb = 5;
	dropboxes[1].value = 0;
	dropboxes[1].worldX = 13;
	dropboxes[1].worldY = 8;
	
	gates[0].worldX = 14;
	gates[0].worldY = 5;
	
	gates[1].worldX = 14;
	gates[1].worldY = 6;
	
	gates[2].worldX = 16;
	gates[2].worldY = 6;
	
	gates[3].worldX = 16;
	gates[3].worldY = 5;
	
	end.worldX = 18;
	end.worldY = 6;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background

	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(StageTutPnPWALLSTiles, StageTutPnPWALLSMap, StageTutPnPTiles, StageTutPnPMap);
	
	// WORLD GRID

	createGrid(StageTutPnPWALLSMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;	
}

void attractModeUdate(int step) {
	player.x = list[step].x;
	player.y = list[step].y;
	
	if (step == 4) {
		boxes[0].worldX = 7;
		boxes[0].worldY = 3;
	}
	
	if (step == 7) {
		boxes[0].worldX = 8;
		boxes[0].worldY = 3;
	}
	
	if (step == 8) {
		boxes[0].worldX = 9;
		boxes[0].worldY = 3;
	}
	
	if (step == 9) {
		boxes[0].worldX = 10;
		boxes[0].worldY = 3;
	}
	
	if (step == 10) {
		boxes[0].worldX = 11;
		boxes[0].worldY = 3;
	}
	
	if (step == 11) {
		boxes[0].worldX = 12;
		boxes[0].worldY = 3;
	}
	
	if (step == 12) {
		boxes[0].worldX = 13;
		boxes[0].worldY = 3;
	}
	
	if (step == 21) {
		boxes[1].worldX = 7;
		boxes[1].worldY = 8;
	}
	
	if (step == 24) {
		boxes[1].worldX = 8;
		boxes[1].worldY = 8;
	}
	
	if (step == 25) {
		boxes[1].worldX = 9;
		boxes[1].worldY = 8;
	}
	
	if (step == 26) {
		boxes[1].worldX = 10;
		boxes[1].worldY = 8;
	}
	
	if (step == 27) {
		boxes[1].worldX = 11;
		boxes[1].worldY = 8;
	}
	
	if (step == 28) {
		boxes[1].worldX = 12;
		boxes[1].worldY = 8;
	}
	
	if (step == 29) {
		boxes[1].worldX = 13;
		boxes[1].worldY = 8;
	}
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
}

int main() {
	gameState = 0;
	
	init();
	char Score[20];
	//char coordinates[20];
	char highScore[20];
	
	menuSelection = 0;
	
	attractModeInit();
	
	int step = 0;

	while(1) {
	
		switch(gameState) {
			case 0: 
				
				tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));

				vid_vsync();
				key_poll();
				
				switch (menuState) {
					case 0: // Main Menu
						REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
						vid_vsync();

						if (key_hit(KEY_START)) {
							menuState = 1;
							step = 0;
							attractModeInit();
						}
						
						tte_write("#{es}");//clear the screen
						tte_write("#{P:56,68}");
						tte_write("#{cx:0x0000}Project Block Boi");
						tte_write("#{P:56,84}");
						tte_write("#{cx:0x0000}->Level Select<-");

						tte_write("#{P:56,100}");
						sprintf(highScore, "#{cx:0x0000}Highscore: %d", hScore);
						
						tte_write(highScore);
						
						int slowSpeed = 15;
						
						int iStep = step / slowSpeed;
						
						attractModeUdate(iStep);
						update();
						draw();
						
						step++;
						iStep = step / slowSpeed;
						
						if (iStep > 38) {
							step = 0;
							attractModeInit();
						}

						break;
						
					case 1: // LEVEL SELECTION
						REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
					
						tte_write("#{es}");
						
						if (levelSelection != 2) {
							if (key_hit(KEY_UP)) {
								if ((levelSelection > 20) || (levelSelection == 12)) {
									levelSelection -= 10;
								}
							}
							
							if (key_hit(KEY_RIGHT)) {
								if ((levelSelection % 10) < 3) {
									levelSelection++;
								}
							}
							
							if (key_hit(KEY_LEFT)) {
								if ((levelSelection % 10) > 1) {
									levelSelection--;
								}
							}
						}
						
						if (key_hit(KEY_DOWN)) {
							if (levelSelection < 20) {
								levelSelection += 10;
							}
						}
						
						if (levelSelection == 2) {
							tte_write("#{P: 96, 56}");
							tte_write("#{cx:0x0000}>Tut <");
						} else {
							tte_write("#{P: 104, 56}");
							tte_write("#{cx:0x0000}Tut");
						}
						
						if (levelSelection == 11) {
							tte_write("#{P: 56, 72}");
							
							if (lvl1Played) {
								tte_write("#{cx:0x0000}>Lvl1<");
							} else {
								tte_write("#{cx:0x8000}>Lvl1<");
							}
						} else {
							tte_write("#{P: 64, 72}");
							
							if (lvl1Played) {
								tte_write("#{cx:0x0000}Lvl1");
							} else {
								tte_write("#{cx:0x8000}Lvl1");
							}
						}
						
						if (levelSelection == 12) {
							tte_write("#{P: 96, 72}");
							
							if (lvl2Played) {
								tte_write("#{cx:0x0000}>Lvl2<");
							} else {
								tte_write("#{cx:0x8000}>Lvl2<");
							}
						} else {
							tte_write("#{P: 104, 72}");
							
							if (lvl2Played) {
								tte_write("#{cx:0x0000}Lvl2");
							} else {
								tte_write("#{cx:0x8000}Lvl2");
							}
						}
						
						if (levelSelection == 13) {
							tte_write("#{P: 136, 72}");
							
							if (lvl3Played) {
								tte_write("#{cx:0x0000}>Lvl3<");
							} else {
								tte_write("#{cx:0x8000}>Lvl3<");
							}
						} else {
							tte_write("#{P: 144, 72}");
							
							if (lvl3Played) {
								tte_write("#{cx:0x0000}Lvl3");
							} else {
								tte_write("#{cx:0x8000}Lvl3");
							}
						}
						
						if (levelSelection == 21) {
							tte_write("#{P: 56, 88}");
							
							if (lvl4Played) {
								tte_write("#{cx:0x0000}>Lvl4<");
							} else {
								tte_write("#{cx:0x8000}>Lvl4<");
							}
						} else {
							tte_write("#{P: 64, 88}");
							
							if (lvl4Played) {
								tte_write("#{cx:0x0000}Lvl4");
							} else {
								tte_write("#{cx:0x8000}Lvl4");
							}
						}
						
						if (levelSelection == 22) {
							tte_write("#{P: 96, 88}");
							
							if (lvl5Played) {
								tte_write("#{cx:0x0000}>Lvl5<");
							} else {
								tte_write("#{cx:0x8000}>Lvl5<");
							}
						} else {
							tte_write("#{P: 104, 88}");
							
							if (lvl5Played) {
								tte_write("#{cx:0x0000}Lvl5");
							} else {
								tte_write("#{cx:0x8000}Lvl5");
							}
						}
						
						if (levelSelection == 23) {
							tte_write("#{P: 136, 88}");
							
							if (lvl6Played) {
								tte_write("#{cx:0x0000}>Lvl6<");
							} else {
								tte_write("#{cx:0x8000}>Lvl6<");
							}
						} else {
							tte_write("#{P: 144, 88}");
							
							if (lvl6Played) {
								tte_write("#{cx:0x0000}Lvl6");
							} else {
								tte_write("#{cx:0x8000}Lvl6");
							}
						}
						
						if (key_hit(KEY_START)) {
							if (levelSelection == 2) {
								gameState = 1;
								initTutorial();
							}
							
							if ((levelSelection == 11) && lvl1Played) {
								gameState = 2;
								initStage1();
							}
							
							if ((levelSelection == 12) && lvl2Played) {
								gameState = 3;
								initStage2();
							}
							
							if ((levelSelection == 13) && lvl3Played) {
								gameState = 4;
								initStage3();
							}
							
							if ((levelSelection == 21) && lvl4Played) {
								gameState = 5;
								initStage4();
							}
							
							if ((levelSelection == 22) && lvl5Played) {
								gameState = 6;
								initStage5();
							}
							
							if ((levelSelection == 23) && lvl6Played) {
								gameState = 7;
								initStage6();
							}
							
						}
						
						break;
				}
				
				break;
				
			case 1: // TUTORIAL
				vid_vsync();
				tte_write("#{es}");//clear the screens
				
				// input();
				update();
				draw();
				
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Tutorial");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);
				
				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
				
				tte_write("#{P:8, 40}");
				tte_write("#{cx:0x0000}Use arrow keys to move");
				
				tte_write("#{P:8, 56}");
				tte_write("#{cx:0x0000}A to push box");
				
				tte_write("#{P:8, 72}");
				tte_write("#{cx:0x0000}B to pull box");
				
				tte_write("#{P:8, 88}");
				tte_write("#{cx:0x0000}Use select to pause");
				
				tte_write("#{P:8, 104}");
				tte_write("#{cx:0x0000}Put boxes in dropboxes");
				
				tte_write("#{P:8, 112}");
				tte_write("#{cx:0x0000}to open gates");
				
				break;
				
			case 2: // LEVEL 1
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 1");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);

				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
		
				
				break;
				
			case 3: // LEVEL 2
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 2");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);

				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
				
				tte_write("#{P:8, 120}");
				tte_write("#{cx:0x0000}Add primary color");
				
				tte_write("#{P:8, 128}");
				tte_write("#{cx:0x0000}boxes together");
				
				tte_write("#{P:8, 136}");
				tte_write("#{cx:0x0000}to match dropbox");
				
				break;
			case 4: // LEVEL 3
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 3");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);
				
				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
				
				tte_write("#{P:8, 120}");
				tte_write("#{cx:0x0000}Add number");
				
				tte_write("#{P:8, 128}");
				tte_write("#{cx:0x0000}boxes together");
				
				tte_write("#{P:8, 136}");
				tte_write("#{cx:0x0000}to match dropbox");
				
				break;
			case 5: // LEVEL 4
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 4");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);

				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
				
				break;
			case 6: // LEVEL 5
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 2");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 5");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);
				
				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
				
				break;
			case 7: // LEVEL 6
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 2");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 6");
				
				//sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				//tte_write("#{P:8, 24}");
				//tte_write(coordinates);
				
				sprintf(Score, "#{cx:0x0000}Score: %d", numStep);
				tte_write("#{P:8,24}");
				tte_write(Score);
				
				break;
				
			case 10: // PAUSE MENU
				vid_vsync();
				tte_write("#{es}");
				
				key_poll(); // checks for key inputs
				
				draw();
				
					tte_write("#{P:104,56}");//56: 72
					tte_write("#{cx:0x0000}Pause");
					
					tte_write("#{P:64,72}");//104: 48
					tte_write("#{cx:0x0000}START to Resume");
					
					tte_write("#{P:48,120}");//152: 48
					tte_write("#{cx:0x0000}Press L: Main menu");
					
					//Have to set the key for "Back to main menu".
					//and set the proper position.
			
					if(key_hit(KEY_START))
					{
						gameState = currentState;
					}
					if(key_hit(KEY_L))
					{
						gameState = 0;
						menuState = 0;
						
						int i;
						for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
							boxes[i].worldX = -64;
							boxes[i].worldY = -64;
						}
						
						for (i = 0; i < NUMBER_DBOXES; i++) {
							dropboxes[i].worldX = -64;
							dropboxes[i].worldY = -64;
						}
						
						for (i = 0; i < NUMBER_GATES; i++) {
							gates[i].worldX = -64;
							gates[i].worldY = -64;
						}
						
						player.x = 6;
						player.y = 5;
						
						end.worldX = 3;
						end.worldY = 13;
						
						backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
						backgroundY = -72 + (8 * player.y);
						
						attractModeInit();
					}
					
					
					
				break;
		}
	}

	return 0;
}