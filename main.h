#ifndef MAIN_H
#define MAIN_H

#define NUMBER_BOXES 10

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

typedef struct Gate {
	OBJ_ATTR *sprite;
	int worldX;
	int worldY;
	int screenX;
	int screenY;
	u32 pb;
	u32 tid;
} Gate;

#endif // MAIN_H