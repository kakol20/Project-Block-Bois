
//{{BLOCK(Stage1Color)

//======================================================================
//
//	Stage1Color, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-05-03, 14:50:28
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE1COLOR_H
#define GRIT_STAGE1COLOR_H

#define Stage1ColorTilesLen 64
extern const unsigned short Stage1ColorTiles[32];

#define Stage1ColorMapLen 2048
extern const unsigned short Stage1ColorMap[1024];

#define Stage1ColorPalLen 512
extern const unsigned short Stage1ColorPal[256];

#endif // GRIT_STAGE1COLOR_H

//}}BLOCK(Stage1Color)
