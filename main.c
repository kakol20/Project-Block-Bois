#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "stage1.h"
#include "player.h"
#include "stage1Floor.h"
#include "boxes.h"
 
 // DECLARATIONS

void draw();
void init();
void input();
void obj_update();

// VARIABLES
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *simplePlayer= &obj_buffer[0];

Box boxes[NUMBER_BOXES];
Player player;

int player_width = 8;
int player_height = 8;

int screen_width = 240;
int screen_height= 160;

int background1_x = 0; 
int background1_y = 0;

void initStage1();
void input();
void obj_update();
void draw();

int gameState = 0;

int menuSelection = 0;
int yDistance;
unsigned short world_grid[32][32];

int main() {
	initStage1();
	
	while(1) {
		switch(gameState) {
			case 0:
				REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
				
				vid_vsync();
				key_poll();
				if (key_hit(KEY_START)) {
					if (menuSelection == 0) {
						gameState = 1;
						
						REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
						//REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
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
				vid_vsync();
				tte_write("#{es}");//clear the screens
			
				char coordinates[50];
				sprintf(coordinates, "#{cx:0x0000}x: %d, y: %d", boxes[0].screenX, yDistance);
				tte_write("#{P:4,8}");
				tte_write(coordinates);
				
				input();
				obj_update();
				draw();
				
				break;
		}
	}
	
	return 0;
}

void initStage1() {
	player.x = 15;
	player.y = 5;
	
	background1_x = -116 + (8 * player.x); // (0, 0) = (-100, -56) // (1, 1) = (-108, -64)
	background1_y = -72 + (8 * player.y);

	// BACKGROUND
	
	tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));
	
	memcpy(pal_bg_mem, stage1Pal, stage1PalLen);
	//memcpy(&tile_mem[1][0], stage1Tiles, stage1TilesLen);
	memcpy(&tile_mem[1][0], stage1Tiles, stage1TilesLen);
	memcpy(&se_mem[30][0], stage1Map, stage1MapLen);
	
	REG_BG1CNT= BG_CBB(1)| BG_SBB(30) | BG_4BPP | BG_REG_64x64;
	
	memcpy(pal_bg_mem, stage1FloorPal, stage1FloorPalLen);
	//memcpy(&tile_mem[2][0], stage1FloorTiles, stage1FloorTilesLen);
	memcpy(&tile_mem[2][0], stage1FloorTiles, stage1FloorTilesLen);
	memcpy(&se_mem[25][0], stage1FloorMap, stage1FloorMapLen);
	
	REG_BG2CNT= BG_CBB(2)| BG_SBB(25) | BG_4BPP | BG_REG_64x64;
	
	/*
	memcpy(pal_bg_mem, floorPal, floorPalLen);
	memcpy(&tile_mem[2][0], floorTiles, floorTilesLen);
	memcpy(&se_mem[29  text is 31, wall is 30 and floor is 29][0], floorMap, floorMapLen);
	
	REG_BG2CNT= BG_CBB(2)| BG_SBB(29) | BG_4BPP | BG_REG_32x32; */
	
	// Sprites
	
	memcpy(&tile_mem[4][0 /*1*/], playerTiles, playerTilesLen);
	memcpy(pal_obj_mem, playerPal, playerPalLen);
	
	player.sprite = &obj_buffer[0];
	player.pb = 0;
	player.tid = 0;
	
	memcpy(&tile_mem[4][1 /*2*/], boxesTiles, boxesTilesLen);
	memcpy(pal_obj_mem, boxesPal, boxesPalLen);
	
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) {	
		boxes[i].sprite = &obj_buffer[1 + i];
		boxes[i].pb = 1;
		boxes[i].tid = 1;
		boxes[i].worldX = 1 + (3 * i);
		boxes[i].worldY = 1;
		boxes[i].screenX =  (boxes[i].worldX * 8)-background1_x; //-116 + (8 * player.worldX);
		boxes[i].screenY =  (boxes[i].worldY * 8)-background1_y; //-72 + (8 * player.worldX);
	}
	
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
	
	
	oam_init(obj_buffer, 128);

	// WORLD GRID
	int xx, yy;
	for (yy = 0; yy < 32; yy++) {
		for (xx = 0; xx < 32; xx++) {
			world_grid[xx][yy] = stage1Map[xx + yy*32];
		}
	}
}

void input() {
	key_poll();

	if ((key_hit(KEY_UP)) && (!world_grid[player.x][player.y - 1]) /* && (player_y >  0) */) {
		background1_y -= 8;
		player.y--;
	}

	if ((key_hit(KEY_DOWN)) && (!world_grid[player.x][player.y + 1]) && (player.y < 31)) {
		background1_y += 8;
		player.y++;
	}

	if ((key_hit(KEY_LEFT)) /* && (!world_grid[player_x - 1][player_y]) */) {
		int ahead = player.x - 1;
		
		if (ahead < 0) {
			ahead = 31;
		}
		
		if (!world_grid[ahead][player.y]) {
			background1_x -= 8;
			player.x = ahead;
		}	
	}
	
	if ((key_hit(KEY_RIGHT)) /* && (!world_grid[player_x + 1][player_y]) && (player_x <= 31) */) {
		int ahead = player.x + 1;
		
		if (ahead > 31) {
			ahead = 0;
		}
		
		if (!world_grid[ahead][player.y]) {
			background1_x += 8;
			player.x = ahead;
		}
	}
	
	if (key_hit(KEY_A)) {
		if (boxes[0].pb == 6) {
			boxes[0].pb = 1;
		} else {
			boxes[0].pb++;
		}
	}
	
	if (key_hit(KEY_B)) {
		if (boxes[1].pb == 6) {
			boxes[1].pb = 1;
		} else {
			boxes[1].pb++;
		}
	}
}

void obj_update() {
	int x = (screen_width - player_width) / 2;
	int y = (screen_height)/2 - player_height;

	// Sprites
	
	obj_set_attr(player.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(player.pb) | player.tid);
	
	obj_set_pos(player.sprite, x, y);
	
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) {	
		boxes[i].screenX =  (boxes[i].worldX * 8)-background1_x;
		
		obj_set_attr(boxes[i].sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(boxes[i].pb) | boxes[i].tid);
		
		yDistance = abs(boxes[i].worldY - player.y);
		if (abs(boxes[i].worldY - player.y) > 15) {
			boxes[i].screenY = -8;
		} else {
			boxes[i].screenY =  (boxes[i].worldY * 8)-background1_y; //-116 + (8 * player.worldX);
		}
		
		obj_set_pos(boxes[i].sprite, boxes[i].screenX, boxes[i].screenY);
	}
	
	oam_copy(oam_mem, obj_buffer, 128 /* how many sprites to load */);
}

void draw() {
	REG_BG1HOFS= background1_x;
	REG_BG1VOFS= background1_y;
	
	REG_BG2HOFS = background1_x;
	REG_BG2VOFS = background1_y;
}
