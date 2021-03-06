#ifndef MAIN_H
#define MAIN_H

#define NUMBER_BOXES 16
#define NUMBER_DBOXES 16
#define NUMBER_GATES 32

typedef struct Box {
	OBJ_ATTR *sprite;
	int worldX;
	int worldY;
	int screenX;
	int screenY;
	int currentColor;
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

typedef struct Sprite {
	OBJ_ATTR *sprite;
	int worldX;
	int worldY;
	int screenX;
	int screenY;
	u32 pb;
	u32 tid;
} Sprite;

typedef struct Coordinates {
	int x;
	int y;
} Coordinates;

#endif // MAIN_H