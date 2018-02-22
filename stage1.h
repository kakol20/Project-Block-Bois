
//{{BLOCK(stage1)

//======================================================================
//
//	stage1, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 4 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 128 + 2048 = 2688
//
//	Time-stamp: 2018-02-22, 17:00:16
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE1_H
#define GRIT_STAGE1_H

#define stage1TilesLen 128
extern const unsigned short stage1Tiles[64];

#define stage1MapLen 2048
extern const unsigned short stage1Map[1024];

#define stage1PalLen 512
extern const unsigned short stage1Pal[256];

#endif // GRIT_STAGE1_H

//}}BLOCK(stage1)
