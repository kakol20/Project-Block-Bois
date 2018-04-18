
//{{BLOCK(tutorial_floor)

//======================================================================
//
//	tutorial_floor, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-12, 16:59:24
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TUTORIAL_FLOOR_H
#define GRIT_TUTORIAL_FLOOR_H

#define tutorial_floorTilesLen 64
extern const unsigned short tutorial_floorTiles[32];

#define tutorial_floorMapLen 2048
extern const unsigned short tutorial_floorMap[1024];

#define tutorial_floorPalLen 512
extern const unsigned short tutorial_floorPal[256];

#endif // GRIT_TUTORIAL_FLOOR_H

//}}BLOCK(tutorial_floor)
