
//{{BLOCK(Stage4)

//======================================================================
//
//	Stage4, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 128 + 2048 = 2688
//
//	Time-stamp: 2018-05-03, 18:54:06
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE4_H
#define GRIT_STAGE4_H

#define Stage4TilesLen 128
extern const unsigned short Stage4Tiles[64];

#define Stage4MapLen 2048
extern const unsigned short Stage4Map[1024];

#define Stage4PalLen 512
extern const unsigned short Stage4Pal[256];

#endif // GRIT_STAGE4_H

//}}BLOCK(Stage4)
