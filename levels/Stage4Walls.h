
//{{BLOCK(Stage4Walls)

//======================================================================
//
//	Stage4Walls, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-05-17, 15:38:56
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE4WALLS_H
#define GRIT_STAGE4WALLS_H

#define Stage4WallsTilesLen 64
extern const unsigned short Stage4WallsTiles[32];

#define Stage4WallsMapLen 2048
extern const unsigned short Stage4WallsMap[1024];

#define Stage4WallsPalLen 512
extern const unsigned short Stage4WallsPal[256];

#endif // GRIT_STAGE4WALLS_H

//}}BLOCK(Stage4Walls)
