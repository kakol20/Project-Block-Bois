
//{{BLOCK(level1_floor)

//======================================================================
//
//	level1_floor, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-12, 15:17:32
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_LEVEL1_FLOOR_H
#define GRIT_LEVEL1_FLOOR_H

#define level1_floorTilesLen 64
extern const unsigned short level1_floorTiles[32];

#define level1_floorMapLen 2048
extern const unsigned short level1_floorMap[1024];

#define level1_floorPalLen 512
extern const unsigned short level1_floorPal[256];

#endif // GRIT_LEVEL1_FLOOR_H

//}}BLOCK(level1_floor)
