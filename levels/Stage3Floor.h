
//{{BLOCK(Stage3Floor)

//======================================================================
//
//	Stage3Floor, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-19, 16:15:39
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE3FLOOR_H
#define GRIT_STAGE3FLOOR_H

#define Stage3FloorTilesLen 64
extern const unsigned short Stage3FloorTiles[32];

#define Stage3FloorMapLen 2048
extern const unsigned short Stage3FloorMap[1024];

#define Stage3FloorPalLen 512
extern const unsigned short Stage3FloorPal[256];

#endif // GRIT_STAGE3FLOOR_H

//}}BLOCK(Stage3Floor)
