
//{{BLOCK(Stage6Both)

//======================================================================
//
//	Stage6Both, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-05-03, 14:52:56
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE6BOTH_H
#define GRIT_STAGE6BOTH_H

#define Stage6BothTilesLen 64
extern const unsigned short Stage6BothTiles[32];

#define Stage6BothMapLen 2048
extern const unsigned short Stage6BothMap[1024];

#define Stage6BothPalLen 512
extern const unsigned short Stage6BothPal[256];

#endif // GRIT_STAGE6BOTH_H

//}}BLOCK(Stage6Both)
