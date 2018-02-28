#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "stage1.h"
#include "player.h"
#include "stage1Floor.h"
#include "boxes/BoxClear.h"

void draw();
// void input();
void update();
// void init();
void initStage1();

// VARTIABLES
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *simplePlayer = &obj_buffer[0];
Box boxes[NUMBER_BOXES];
Player player;

int screenWidth = 240;
int screenHeight = 160;
int backgroundX = 0;
int backgroundY = 0;

int gameState = 0;
int menuSelection = 0;
int xDistance;
int yDistance;

unsigned short world_grid[32][32];

void initStage1() {
	player.width = 8;
	player.height = 8;
	player.x = 15;
	player.y = 5;

	backgroundX = -116 + (8 * player.x);
	backgroundY = -72 + (8 * player.y);

	// BACKGROUND

	tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));

	memcpy(pal_bg_mem, stage1Pal, stage1PalLen);
	memcpy(&tile_mem[1][0], stage1Tiles, stage1TilesLen);
	memcpy(&se_mem[30][0], stage1Map, stage1MapLen);
	REG_BG1CNT = BG_CBB(1) | BG_SBB(30) | BG_4BPP | BG_REG_64x64;

	memcpy(pal_bg_mem, stage1FloorPal, stage1FloorPalLen);
	memcpy(&tile_mem[2][0], stage1FloorTiles, stage1FloorTilesLen);
	memcpy(&se_mem[25][0], stage1FloorMap, stage1FloorMapLen);
	REG_BG2CNT = BG_CBB(2) | BG_SBB(25) | BG_4BPP | BG_REG_64x64;

	// SPRITES

	memcpy(&tile_mem[4][0 /*1*/], playerTiles, playerTilesLen);
	memcpy(pal_obj_mem, playerPal, playerPalLen);
	player.sprite = &obj_buffer[0];
	player.pb = 0;
	player.tid = 0;

	memcpy(&tile_mem[4][1 /*2*/], BoxClearTiles, BoxClearTilesLen);
	memcpy(pal_obj_mem, BoxClearPal, BoxClearPalLen);
	int i;
	for (i = 0; i < NUMBER_BOXES; i++) {
		boxes[i].sprite = &obj_buffer[i + 1];
		boxes[i].pb = 1;
		boxes[i].tid = 1;
		boxes[i].worldX = 1 + (i * 3);
		boxes[i].worldY = 1;
		boxes[i].screenX = (boxes[i].worldX * 8) - backgroundX;
		boxes[i].screenY = (boxes[i].worldY * 8) - backgroundY;
	}

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	oam_init(obj_buffer, 128);

	// WORLD GRID

	int xx, yy;
	for (yy = 0; yy < 32; yy++) {
		for (xx = 0; xx < 32; xx++) {
			world_grid[xx][yy] = stage1Map[xx + (yy * 32)];
		}
	}
}

void update() {
	// INPUT 

	key_poll();

	if ((key_hit(KEY_UP)) && (!world_grid[player.x][player.y - 1]) /* && (player_y >  0) */) {
		backgroundY -= 8;
		player.y--;
	}

	if ((key_hit(KEY_DOWN)) && (!world_grid[player.x][player.y + 1]) && (player.y < 31)) {
		backgroundY += 8;
		player.y++;
	}

	if ((key_hit(KEY_LEFT)) /* && (!world_grid[player_x - 1][player_y]) */) {
		int ahead = player.x - 1;
		
		if (ahead < 0) {
			ahead = 31;
		}
		
		if (!world_grid[ahead][player.y]) {
			backgroundX -= 8;
			player.x = ahead;
		}	
	}
	
	if ((key_hit(KEY_RIGHT)) /* && (!world_grid[player_x + 1][player_y]) && (player_x <= 31) */) {
		int ahead = player.x + 1;
		
		if (ahead > 31) {
			ahead = 0;
		}
		
		if (!world_grid[ahead][player.y]) {
			backgroundX += 8;
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

void draw() {
	// SPRITES

	int x = (screenWidth - player.width) / 2;
	int y = (screenHeight / 2) - player.height;
	obj_set_attr(player.sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(player.pb) | player.tid);
	obj_set_pos(player.sprite, x, y);

	int i;
	for (i = 0; i < NUMBER_BOXES; i++) {
		obj_set_attr(boxes[i].sprite, ATTR0_SQUARE, ATTR1_SIZE_8, ATTR2_PALBANK(boxes[i].pb) | boxes[i].tid);

		xDistance = abs(boxes[i].worldX - player.x);
		yDistance = abs(boxes[i].worldY - player.y);

		if ((xDistance > 16) || (yDistance > 16)) {
			boxes[i].screenX = -8;
			boxes[i].screenY = -8;
		} else {
			boxes[i].screenX = (boxes[i].worldX * 8) - backgroundX;
			boxes[i].screenY = (boxes[i].worldY * 8) - backgroundY;
		}

		obj_set_pos(boxes[i].sprite, boxes[i].screenX, boxes[i].screenY);
	}

	oam_copy(oam_mem, obj_buffer, 128);

	// BACKGROUND

	REG_BG1HOFS= backgroundX;
	REG_BG1VOFS= backgroundY;
	
	REG_BG2HOFS = backgroundX;
	REG_BG2VOFS = backgroundY;
}

int main() {
	// initStage1();

	while(1) {
		switch(gameState) {
			case 0:
				REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

				vid_vsync();
				key_poll();

				if (key_hit(KEY_START)) {
					if (menuSelection == 0) {
						gameState = 1;

						initStage1();

						REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
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
				tte_write("#{P:0,0}");
				tte_write(coordinates);
				
				// input();
				update();
				draw();
				
				break;
		}
	}

	return 0;
}