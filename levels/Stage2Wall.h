
//{{BLOCK(Stage2Wall)

//======================================================================
//
//	Stage2Wall, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-19, 16:15:10
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE2WALL_H
#define GRIT_STAGE2WALL_H

#define Stage2WallTilesLen 64
extern const unsigned short Stage2WallTiles[32];

#define Stage2WallMapLen 2048
extern const unsigned short Stage2WallMap[1024];

#define Stage2WallPalLen 512
extern const unsigned short Stage2WallPal[256];

#endif // GRIT_STAGE2WALL_H

//}}BLOCK(Stage2Wall)
