
//{{BLOCK(Stage1Wall)

//======================================================================
//
//	Stage1Wall, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-19, 16:10:08
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE1WALL_H
#define GRIT_STAGE1WALL_H

#define Stage1WallTilesLen 64
extern const unsigned short Stage1WallTiles[32];

#define Stage1WallMapLen 2048
extern const unsigned short Stage1WallMap[1024];

#define Stage1WallPalLen 512
extern const unsigned short Stage1WallPal[256];

#endif // GRIT_STAGE1WALL_H

//}}BLOCK(Stage1Wall)
