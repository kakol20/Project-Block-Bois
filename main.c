#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "player.h"
#include "stage1.h"
#include "stage1Floor.h"
#include "stage2.h"
#include "stage2Floor.h"
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
void initStage1();
void initStage2();
void createGrid(const unsigned short *map);
void addBackground(const unsigned short *wallTiles, const unsigned short *wallMap, const unsigned short *floorTiles, const unsigned short *floorMap);
int boxCollision(int x, int y);

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
int nextBuffer;

unsigned short world_grid[32][32];

int boxCollision(int x, int y) {
	int i;
	for (i = 0; i < NUMBER_BOXES;  i++) {
		if ((x == boxes[i].worldX) && (y == boxes[i].worldY)) { // Checks if there is a box on the x,y position
			return i; // returns the index in the boxes[] array
		}
	}
	
	return -1; // returns -1 if nothing was found
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
	memcpy(pal_bg_mem, stage1Pal, stage1PalLen /* Should be the same for all levels*/);
	memcpy(&tile_mem[1][0], wallTiles, stage1TilesLen /* Should be the same for all levels*/);
	memcpy(&se_mem[30][0], wallMap, stage1MapLen /* Should be the same for all levels*/);
	REG_BG1CNT = BG_CBB(1) | BG_SBB(30) | BG_4BPP | BG_REG_64x64;
	
	memcpy(&tile_mem[2][0], floorTiles, stage1FloorTilesLen /* Should be the same for all levels*/);
	memcpy(&se_mem[25][0], floorMap, stage1FloorMapLen /* Should be the same for all levels*/);
	REG_BG2CNT = BG_CBB(2) | BG_SBB(25) | BG_4BPP | BG_REG_64x64; // Using 64 x 64 despite map being 32 x 32 to avoid the map from being repeated
}

void init() {
	memcpy(pal_bg_mem, stage1Pal, stage1PalLen /* Should be the same for all levels*/);

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
		boxes[i].pb = 1; // red color
		
		boxes[i].value = i;
		boxes[i].tid = boxes[i].value + 1;
		
		boxes[i].worldX = 1 + (i * 3);
		boxes[i].worldY = 1 + i;
		boxes[i].screenX = (boxes[i].worldX * 8) - backgroundX; // sets in position on the screen
		boxes[i].screenY = (boxes[i].worldY * 8) - backgroundY;
		nextBuffer = i + 2;
	}
	
	memcpy(&tile_mem[4][11], tempGateTiles, tempGateTilesLen);
	memcpy(pal_obj_mem, tempGatePal, tempGatePalLen);
	gate.sprite = &obj_buffer[nextBuffer];
	gate.pb = 0;
	gate.tid = 11;
	gate.worldX = 4;
	gate.worldY = 19;

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	oam_init(obj_buffer, 128);
}

void initStage1() { // replace any existing map with the this level
	player.width = 8;
	player.height = 8;
	player.x = 4;
	player.y = 4;

	backgroundX = -116 + (8 * player.x); // changes the background's position based on the player's world position
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND - adding background
	
	addBackground(stage1Tiles, stage1Map, stage1FloorTiles, stage1FloorMap);
	
	// WORLD GRID

	createGrid(stage1Map);
	
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
}

void initStage2() {
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
	
	if (key_hit(KEY_UP)) {
		bool move = false;
		// store the current box and the box ahead - store it's index
		
		
		if (!world_grid[player.x][player.y - 1]) {
			// check if the boxes are compatible
			// red = 1, orange = 2, yellow = 3, green = 4, blue = 5, purple = 6 pallette bank
			// if both are odd, they are compatible
			// if statements
			// x + y > 9 not compatible
			if (key_held(KEY_A) && (boxCollision(player.x, player.y - 1) >= 0) && !world_grid[player.x][player.y - 2] && (boxCollision(player.x, player.y - 2) < 0)) { // checks if the player will be able to push the box, to check if there is an obstacle in the way
				boxes[boxCollision(player.x, player.y - 1)].worldY--; // pushing
				
				move = true;
			} else if (key_held(KEY_B) && (boxCollision(player.x, player.y + 1) >= 0) && (boxCollision(player.x, player.y - 1) < 0)) {
				boxes[boxCollision(player.x, player.y + 1)].worldY--; // pulling
				
				move = true;
			} else if (boxCollision(player.x, player.y - 1) < 0) { // player can move if the there is nothing in the way
				move = true;
			}
		}
		
		if (move) {
			backgroundY -= 8; 
			player.y--;
		}
	}
	
	if (key_hit(KEY_DOWN)) { // same as key up
		bool move = false;
		
		if (!world_grid[player.x][player.y + 1]) {
			if (key_held(KEY_A) && (boxCollision(player.x, player.y + 1) >= 0) && !world_grid[player.x][player.y + 2] && (boxCollision(player.x, player.y + 2) < 0)) {
				boxes[boxCollision(player.x, player.y + 1)].worldY++;
				
				move = true;
			} else if (key_held(KEY_B) && (boxCollision(player.x, player.y - 1) >= 0) && (boxCollision(player.x, player.y + 1) < 0)) {
				boxes[boxCollision(player.x, player.y - 1)].worldY++;
				
				move = true;
			} else if (boxCollision(player.x, player.y + 1) < 0) {
				move = true;
			}
		}
		
		if (move) {
			backgroundY += 8;
			player.y++;
		}
	}
	
	if (key_hit(KEY_LEFT)) { // same as key up
		bool move = false;
		
		int ahead = player.x - 1;
		
		if (ahead < 0) {
			ahead = 31;
		}
		
		if (!world_grid[ahead][player.y]) {
			if (key_held(KEY_A) && (boxCollision(ahead, player.y) >= 0) && !world_grid[ahead - 1][player.y] && (boxCollision(ahead - 1, player.y) < 0)) {
				boxes[boxCollision(ahead, player.y)].worldX--;;
				
				move = true;
			} else if (key_held(KEY_B) && (boxCollision(player.x + 1, player.y) >= 0) && (boxCollision(ahead, player.y) < 0)) {
				boxes[boxCollision(player.x + 1, player.y)].worldX--;
				
				move = true;
			} else if (boxCollision(ahead, player.y) < 0) {
				move = true;
			}
		}
		
		if (move) {
			backgroundX -= 8;
			player.x = ahead;
		}
	}
	
	if (key_hit(KEY_RIGHT)) { // same as key up
		bool move = false;
		
		int ahead = player.x + 1;
		
		if (ahead > 31) {
			ahead = 0;
		}
		
		if (!world_grid[ahead][player.y]) {
			if (key_held(KEY_A) && (boxCollision(player.x + 1, player.y) >= 0) && !world_grid[ahead + 1][player.y] && (boxCollision(ahead + 1, player.y) < 0)) {
				boxes[boxCollision(player.x + 1, player.y)].worldX++;
				
				move = true;
			} 
			else if (key_held(KEY_B) && (boxCollision(player.x - 1, player.y) >= 0) && (boxCollision(player.x + 1, player.y) < 0)) {
				boxes[boxCollision(player.x - 1, player.y)].worldX++;
				
				move = true;
			}
			else if (boxCollision(player.x + 1, player.y) < 0) {
				move = true;
			}
		}
		
		if (move) {
			backgroundX += 8;
			player.x = ahead;
		}
	}

	// MOVING TO ANOTHER MAP

	if ((gate.worldX == player.x) && (gate.worldY == player.y)) { // tempory movement between maps - testing if the player walks over the gate, then it will go to another map
		switch (gameState) {
			case 1: // level 1 - gameState 0 is the main menu
				gameState = 2;
				initStage2();
				
				break;
				
			case 2:
				gameState = 1;
				initStage1();
				
				break;
		}
	}
}

void draw() {	
	int x = (screenWidth - player.width) / 2; // player sprite
	int y = (screenHeight / 2) - player.height;
	obj_set_attr(player.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(player.pb) | player.tid);
	obj_set_pos(player.sprite, x, y); // puts the player on the screen

	int i;
	for (i = 0; i < NUMBER_BOXES; i++) { // goes through boxes[] array and draws it onto the screen
		boxes[i].tid = boxes[i].value + 1;
	
		obj_set_attr(boxes[i].sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(boxes[i].pb) | boxes[i].tid);

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
	obj_set_attr(gate.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(gate.pb) | gate.tid);
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
						initStage1();
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
			
				sprintf(coordinates, "#{cx:0x0000}Level 1");
				tte_write("#{P:1,1}");
				tte_write(coordinates);
				
				// input();
				update();
				draw();
				
				break;
				
			case 2: // stage 2 game state
				vid_vsync();
				tte_write("#{es}");
			
				sprintf(coordinates, "#{cx:0x0000}Level 2");
				tte_write("#{P:1,1}");
				tte_write(coordinates);
				
				// input();
				update();
				draw();
				
				break;
				
		}
	}

	return 0;
}