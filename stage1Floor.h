
//{{BLOCK(stage1Floor)

//======================================================================
//
//	stage1Floor, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 5 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 160 + 2048 = 2720
//
//	Time-stamp: 2018-02-22, 16:59:04
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE1FLOOR_H
#define GRIT_STAGE1FLOOR_H

#define stage1FloorTilesLen 160
extern const unsigned short stage1FloorTiles[80];

#define stage1FloorMapLen 2048
extern const unsigned short stage1FloorMap[1024];

#define stage1FloorPalLen 512
extern const unsigned short stage1FloorPal[256];

#endif // GRIT_STAGE1FLOOR_H

//}}BLOCK(stage1Floor)
