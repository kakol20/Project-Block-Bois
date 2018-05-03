#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "player.h"

// LEVELS
#include "levels/tutorial_walls.h"
#include "levels/tutorial_floor.h"
#include "levels/Stage1Floor.h"
#include "levels/Stage1Wall.h"
#include "levels/Stage2Floor.h"
#include "levels/Stage2Wall.h"

#include "levels/Stage1Color.h"
#include "levels/Stage1ColorWalls.h"
#include "levels/Stage2ColorCombo.h"
#include "levels/Stage2ColorComboWALL.h"
#include "levels/Stage3NumberCombo.h"
#include "levels/Stage3NumberComboWalls.h"
#include "levels/Stage4.h"
#include "levels/Stage4Walls.h"
#include "levels/Stage5new.h"
#include "levels/Stage4newWalls.h"

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

void createGrid(const unsigned short *map);
void addBackground(const unsigned short *wallTiles, const unsigned short *wallMap, const unsigned short *floorTiles, const unsigned short *floorMap);
void move(int changeX, int changeY);
bool merge(int boxToMerge);
bool isOdd(int n);
int collision(int x, int y, bool mergeBoxes);

// VARTIABLES
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *simplePlayer = &obj_buffer[0];

Box boxes[NUMBER_BOXES]; // boxes
Box dropboxes[NUMBER_DBOXES]; // dropboxes

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
	memcpy(pal_bg_mem, tutorial_wallsPal, tutorial_wallsPalLen /* Should be the same for all levels*/);
	memcpy(&tile_mem[1][0], wallTiles, tutorial_wallsTilesLen /* Should be the same for all levels*/);
	memcpy(&se_mem[30][0], wallMap, tutorial_wallsMapLen /* Should be the same for all levels*/);
	REG_BG1CNT = BG_CBB(1) | BG_SBB(30) | BG_4BPP | BG_REG_64x64 | BG_PRIO(1);
	
	memcpy(&tile_mem[2][0], floorTiles,tutorial_floorTilesLen /* Should be the same for all levels*/);
	memcpy(&se_mem[25][0], floorMap, tutorial_floorMapLen /* Should be the same for all levels*/);
	REG_BG2CNT = BG_CBB(2) | BG_SBB(25) | BG_4BPP | BG_REG_64x64 | BG_PRIO(1); // Using 64 x 64 despite map being 32 x 32 to avoid the map from being repeated
}

void init() {
	memcpy(pal_bg_mem, tutorial_wallsPal, tutorial_wallsPalLen /* Should be the same for all levels*/);

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

	player.x = 1;
	player.y = 4;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// Sprites
	
	boxes[0].worldX = 5; // places the boxes
	boxes[0].worldY = 3;
	boxes[0].pb = 1;
	boxes[0].value = 0;
	
	boxes[1].worldX = 20;
	boxes[1].worldY = 11;
	boxes[1].pb = 3;
	boxes[1].value = 0;
	
	boxes[2].worldX = 20;
	boxes[2].worldY = 14;
	boxes[2].pb = 5;
	boxes[2].value = 0;
	
	for (i = 3; i < NUMBER_BOXES; i++) { // goes through the other boxes and places them out of the screen
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}
	
	end.worldX = 11; // places the end
	end.worldY = 25;
	
	dropboxes[0].worldX = 5; // places the dropboxes
	dropboxes[0].worldY = 6;
	dropboxes[0].pb = 1;
	dropboxes[0].value = 0;
	
	dropboxes[1].worldX = 23;
	dropboxes[1].worldY = 14;
	dropboxes[1].pb = 3;
	dropboxes[1].value = 0;
	
	for (i = 2; i < NUMBER_DBOXES; i++) { // goes through the other drop boxes and places them out of the screen
		dropboxes[i].worldX = -64;
		dropboxes[i].worldY = -64;
	}
	
	int index = 0;
	
	int j;
	
	for (i = 11; i <= 16; i++) { // places the gates
		for (j = 2; j <= 3; j++) {
			gates[index].worldX = i;
			gates[index].worldY = (j == 2) ? 1 : 4;
			
			index++;
		}
	}
	
	for (i = 21; i <= 22; i++) {
		gates[index].worldX = (i == 21) ? 20 : 23;
		gates[index].worldY = 20;
		
		index++;
	}
	
	for (i = index; i < NUMBER_GATES; i++) { // goes through the other gates and places them out of the screen
		gates[i].worldX = -64;
		gates[i].worldY = -64;
	
	}
	
	// BACKGROUND - adding background
	
	addBackground(tutorial_wallsTiles, tutorial_wallsMap, tutorial_floorTiles, tutorial_floorMap);
	
	// WORLD GRID

	createGrid(tutorial_wallsMap);
	
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

	player.x = 1;
	player.y = 5;
	
	end.worldX = 6;
	end.worldY = 11;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background

	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage1WallTiles, Stage1WallMap, Stage1FloorTiles, Stage1FloorMap);
	
	// WORLD GRID

	createGrid(Stage1WallMap);
	
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
	
	player.x = 3;
	player.y = 3;
	
	end.worldX = 16;
	end.worldY = 16;
	
	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);
	
	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(Stage2WallTiles, Stage2WallMap, Stage2FloorTiles, Stage2FloorMap);
	
	// WORLD GRID

	createGrid(Stage2WallMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
	
}

void update() {
	// INPUT 

	key_poll(); // checks for key inputs
	
	int changeX = 0;
	int changeY = 0;
	
	if (key_hit(KEY_UP)) {
		changeY = -1;// parameters are how much should the player position change
	}
	
	if (key_hit(KEY_RIGHT)) {
		changeX = 1;
	}
	
	if (key_hit(KEY_DOWN)) {
		changeY = 1;
	}
	
	if (key_hit(KEY_LEFT)) {
		changeX = -1;
	}
	
	move(changeX, changeY);
	
	// MOVING TO ANOTHER MAP

	if ((end.worldX == player.x) && (end.worldY == player.y)) { // tempory movement between maps - testing if the player walks over the end, then it will go to another map
		switch (gameState) {
			case 1: // tutorial - gameState 0 is the main menu
				gameState = 2;
				initStage1();
				
				break;
			case 2: // level 1
				gameState = 3;
				initStage2();
				
				break;
			case 3: // level 2
				gameState = 1;
				initTutorial();
				
				break;
		}
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

int main() {
	gameState = 0;
	
	init();
	char coordinates[50];
	menuSelection = 0;
	
	int menuState= 0;

	while(1) {
	
		switch(gameState) {
			case 0:
				REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
				
				tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));

				vid_vsync();
				key_poll();
				
				switch (menuState) {
					case 0: // Main Menu
						if (key_hit(KEY_START)) {
							if (menuSelection == 0) {
								menuState = 1;
								
							}
						}

						if (key_hit(KEY_DOWN)) {
							menuSelection = 1;
						}

						if (key_hit(KEY_UP)) {
							menuSelection = 0;
						}

						
						if (menuSelection == 0) {
							tte_write("#{es}");//clear the screen
							tte_write("#{P:52,68}");
							tte_write("#{cx:0x0000}Project Block Boi");
							tte_write("#{P:52,84}");
							tte_write("#{cx:0x0000}->Level Select <-");
							tte_write("#{P:42,100}");
							tte_write("#{cx:0x0000}   Leaderboard");
						} else if (menuSelection == 1) {
							tte_write("#{es}");//clear the screen
							
							tte_write("#{P:52,68}");
							tte_write("#{cx:0x0000}Project Block Boi");
							tte_write("#{P:52,84}");
							tte_write("#{cx:0x0000}  Level Select");
							tte_write("#{P:52,100}");
							tte_write("#{cx:0x0000}->Leaderboard  <-");
						}
						
						break;
						
					case 1: 
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
							if (levelSelection < 30) {
								levelSelection += 10;
							}
						}
						
						if (levelSelection == 2) { // tutorial level
							tte_write("#{P: 96, 48}");
							tte_write("#{cx:0x0000}>Tut <");
						} else {
							tte_write("#{P: 104, 48}");
							tte_write("#{cx:0x0000}Tut");
						}
						
						if (levelSelection == 11) { // level 1 stage 1
							tte_write("#{P: 56, 64}");
							tte_write("#{cx:0x0000}>Lvl1<");
						} else {
							tte_write("#{P: 64, 64}");
							tte_write("#{cx:0x0000}Lvl1");
						}
						
						if (levelSelection == 12) { // level 1 stage 2
							tte_write("#{P: 96, 64}");
							tte_write("#{cx:0x0000}>Lvl2<");
						} else {
							tte_write("#{P: 104, 64}");
							tte_write("#{cx:0x0000}Lvl2");
						}
						
						if (levelSelection == 13) { // level 1 stage 3
							tte_write("#{P: 136, 64}");
							tte_write("#{cx:0x0000}>Lvl3<");
						} else {
							tte_write("#{P: 144, 64}");
							tte_write("#{cx:0x0000}Lvl3");
						}
						
						if (levelSelection == 21) { // level 2 stage 1
							tte_write("#{P: 56, 80}");
							tte_write("#{cx:0x0000}>Lvl4<");
						} else {
							tte_write("#{P: 64, 80}");
							tte_write("#{cx:0x0000}Lvl4");
						}
						
						if (levelSelection == 22) { // level 2 stage 2
							tte_write("#{P: 96, 80}");
							tte_write("#{cx:0x0000}>Lvl5<");
						} else {
							tte_write("#{P: 104, 80}");
							tte_write("#{cx:0x0000}Lvl5");
						}
						
						if (levelSelection == 23) { // level 2 stage 3
							tte_write("#{P: 136, 80}");
							tte_write("#{cx:0x0000}>Lvl6<");
						} else {
							tte_write("#{P: 144, 80}");
							tte_write("#{cx:0x0000}Lvl6");
						}
						
						if (levelSelection == 31) {
							tte_write("#{P: 56, 96}");
							tte_write("#{cx:0x0000}>Lvl7<");
						} else {
							tte_write("#{P: 64, 96}");
							tte_write("#{cx:0x0000}Lvl7");
						}
						
						if (levelSelection == 32) {
							tte_write("#{P: 96, 96}");
							tte_write("#{cx:0x0000}>Lvl8<");
						} else {
							tte_write("#{P: 104, 96}");
							tte_write("#{cx:0x0000}Lvl8");
						}
						
						if (levelSelection == 33) {
							tte_write("#{P: 136s, 96}");
							tte_write("#{cx:0x0000}>Lvl9<");
						} else {
							tte_write("#{P: 144, 96}");
							tte_write("#{cx:0x0000}Lvl9");
						}
						
						if (key_hit(KEY_START)) {
							if (levelSelection == 2) {
								gameState = 1;
								initTutorial();
							}
							
							if (levelSelection == 11) {
								gameState = 2;
								initStage1();
							}
							
							if (levelSelection == 12) {
								gameState = 3;
								initStage2();
							}
						}
						
						break;
				}

				break;
				
			case 1: // tutorial game state
				vid_vsync();
				tte_write("#{es}");//clear the screens
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 1");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Tutorial");
				
				sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				tte_write("#{P:8, 24}");
				tte_write(coordinates);
				
				break;
				
			case 2: // stage 1 game state
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 2");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 1");
				
				sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				tte_write("#{P:8, 24}");
				tte_write(coordinates);
				
				break;
				
			case 3: // stage 2 game state
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 2");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Stage 2");
				
				sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				tte_write("#{P:8, 24}");
				tte_write(coordinates);
				
				break;
		}
	}

	return 0;
}