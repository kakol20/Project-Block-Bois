#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "player.h"

#include "stage2.h"
#include "stage2Floor.h"

// LEVELS
#include "levels/tutorial_walls.h"
#include "levels/tutorial_floor.h"

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
#include "tempGate.h"

void draw();
void update();
void nextLevel();
void init();
void initTutorial();
void initStage2();
void createGrid(const unsigned short *map);
void addBackground(const unsigned short *wallTiles, const unsigned short *wallMap, const unsigned short *floorTiles, const unsigned short *floorMap);
void move(int changeX, int changeY);
bool merge(int boxToMerge);
bool isOdd(int n);
int boxCollision(int x, int y, bool mergeBoxes);

// VARTIABLES
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *simplePlayer = &obj_buffer[0];
Box boxes[NUMBER_BOXES];
Player player;
Gate gate;

int screenWidth = 240;
int screenHeight = 160;
int backgroundX = 0;
int backgroundY = 0;

int gameState = 0;
int menuSelection = 0;
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

int boxCollision(int x, int y, bool mergeBoxes) {

	int i;
	for (i = 0; i < NUMBER_BOXES;  i++) {
		if ((x == boxes[i].worldX) && (y == boxes[i].worldY)) { // Checks if there is a box on the x,y position
			if (mergeBoxes) {
				if (merge(i)) {
					return -1; // return -1 to say that it can move
				}
			}
		
			return i; // returns the index in the boxes[] array
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
			currBox = boxCollision(player.x + changeX, player.y + changeY, false); // pushing
			
			if ((currBox >= 0) && !world_grid[player.x + changeX + changeX][player.y + changeY + changeY] && (boxCollision(player.x + changeX + changeX, player.y + changeY + changeY, true) < 0)) {
				
				// example move(0, 1)
				// currBox = (player x position, player y position + 1)
				// box ahea of currBox = (player x position, player y position + 2
				
				boxes[currBox].worldX += changeX;
				boxes[currBox].worldY += changeY;	
				move = true;
			}
		}  if (key_held(KEY_B)) { // pulling
			currBox = boxCollision(player.x - changeX, player.y - changeY, false);
			
			if ((currBox >= 0) && (boxCollision(player.x + changeX, player.y + changeY, false) < 0)) {
				boxes[currBox].worldX += changeX;
				boxes[currBox].worldY += changeY;
				move = true;
			}
		} else if (boxCollision(player.x + changeX, player.y + changeY, false) < 0){
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
		boxes[i].pb = (i % 6) + 1;
		
		boxes[i].value = 0;
		boxes[i].tid = 1;
		
		boxes[i].worldX = -64;
		boxes[i].worldY = -64 + i;
		boxes[i].screenX = (boxes[i].worldX * 8) - backgroundX; // sets in position on the screen
		boxes[i].screenY = (boxes[i].worldY * 8) - backgroundY;
		nextBuffer = i + 2;
	}
	
	memcpy(&tile_mem[4][11], tempGateTiles, tempGateTilesLen);
	memcpy(pal_obj_mem, tempGatePal, tempGatePalLen);
	gate.sprite = &obj_buffer[nextBuffer];
	gate.pb = 0;
	gate.tid = 11;
	gate.worldX = -64;
	gate.worldY = -64;

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	oam_init(obj_buffer, 128);
}

void initTutorial() { // replace any existing map with the this level
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}

	player.width = 8;
	player.height = 8;
	player.x = 1;
	player.y = 4;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background
	
	addBackground(tutorial_wallsTiles, tutorial_wallsMap, tutorial_floorTiles, tutorial_floorMap);
	
	boxes[0].worldX = 5;
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
	
	
	// WORLD GRID

	createGrid(tutorial_wallsMap);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

void initStage2() {
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // reset every box to out of the map
		boxes[i].worldX = -64;
		boxes[i].worldY = -64;
	}

	player.width = 8;
	player.height = 8;
	player.x = 4;
	player.y = 4;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background

	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
	
	addBackground(stage2Tiles, stage2Map, stage2FloorTiles, stage2FloorMap);
	
	// WORLD GRID

	createGrid(stage2Map);
	
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

	if ((gate.worldX == player.x) && (gate.worldY == player.y)) { // tempory movement between maps - testing if the player walks over the gate, then it will go to another map
		switch (gameState) {
			case 1: // level 1 - gameState 0 is the main menu
				gameState = 2;
				initStage2();
				
				break;
				
			case 2:
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
	
	xDistance = abs(gate.worldX - player.x);
	yDistance = abs(gate.worldY - player.y);
	obj_set_attr(gate.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(gate.pb) | gate.tid | ATTR2_PRIO(1));
	if ((xDistance > 16) || (yDistance > 16)) {
		gate.screenX = -8;
		gate.screenY = -8;
	} else {
		gate.screenX = (gate.worldX * 8) - backgroundX;
		gate.screenY = (gate.worldY * 8) - backgroundY;
	}
	obj_set_pos(gate.sprite, gate.screenX, gate.screenY);

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

	while(1) {
	
		switch(gameState) {
			case 0:
				REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
				
				tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));

				vid_vsync();
				key_poll();

				if (key_hit(KEY_START)) {
					if (menuSelection == 0) {
						gameState = 1;
						initTutorial();
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
				
			case 1: // stage 1 game state
				vid_vsync();
				tte_write("#{es}");//clear the screens
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 1");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Level 1");
				
				sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				tte_write("#{P:8, 24}");
				tte_write(coordinates);
				
				break;
				
			case 2: // stage 2 game state
				vid_vsync();
				tte_write("#{es}");
				
				// input();
				update();
				draw();
				
				// sprintf(coordinates, "#{cx:0x0000}Level 2");
				tte_write("#{P:8,8}");
				tte_write("#{cx:0x0000}Level 2");
				
				sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", player.x, player.y);
				tte_write("#{P:8, 24}");
				tte_write(coordinates);
				
				break;
				
		}
	}

	return 0;
}