
//{{BLOCK(Stage3Wall)

//======================================================================
//
//	Stage3Wall, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-19, 16:16:15
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE3WALL_H
#define GRIT_STAGE3WALL_H

#define Stage3WallTilesLen 64
extern const unsigned short Stage3WallTiles[32];

#define Stage3WallMapLen 2048
extern const unsigned short Stage3WallMap[1024];

#define Stage3WallPalLen 512
extern const unsigned short Stage3WallPal[256];

#endif // GRIT_STAGE3WALL_H

//}}BLOCK(Stage3Wall)
