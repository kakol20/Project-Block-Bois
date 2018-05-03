
//{{BLOCK(Stage9Both)

//======================================================================
//
//	Stage9Both, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-05-03, 14:55:19
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE9BOTH_H
#define GRIT_STAGE9BOTH_H

#define Stage9BothTilesLen 64
extern const unsigned short Stage9BothTiles[32];

#define Stage9BothMapLen 2048
extern const unsigned short Stage9BothMap[1024];

#define Stage9BothPalLen 512
extern const unsigned short Stage9BothPal[256];

#endif // GRIT_STAGE9BOTH_H

//}}BLOCK(Stage9Both)
