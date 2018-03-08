
//{{BLOCK(stage2Floor)

//======================================================================
//
//	stage2Floor, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-03-08, 15:58:51
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE2FLOOR_H
#define GRIT_STAGE2FLOOR_H

#define stage2FloorTilesLen 64
extern const unsigned short stage2FloorTiles[32];

#define stage2FloorMapLen 2048
extern const unsigned short stage2FloorMap[1024];

#define stage2FloorPalLen 512
extern const unsigned short stage2FloorPal[256];

#endif // GRIT_STAGE2FLOOR_H

//}}BLOCK(stage2Floor)
