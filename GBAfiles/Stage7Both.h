
//{{BLOCK(Stage7Both)

//======================================================================
//
//	Stage7Both, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-05-03, 14:54:40
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE7BOTH_H
#define GRIT_STAGE7BOTH_H

#define Stage7BothTilesLen 64
extern const unsigned short Stage7BothTiles[32];

#define Stage7BothMapLen 2048
extern const unsigned short Stage7BothMap[1024];

#define Stage7BothPalLen 512
extern const unsigned short Stage7BothPal[256];

#endif // GRIT_STAGE7BOTH_H

//}}BLOCK(Stage7Both)
