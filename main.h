#ifndef MAIN_H
#define MAIN_H

#define NUMBER_BOXES 2

typedef struct Box {
	OBJ_ATTR *sprite;
	int worldX;
	int worldY;
	int screenX;
	int screenY;
	u32 pb;
	u32 tid;
	int value;
} Box;

typedef struct Player {
	OBJ_ATTR *sprite;
	int x;
	int y;
	int width;
	int height;
	u32 pb;
	u32 tid;
} Player;

#endif // MAIN_H