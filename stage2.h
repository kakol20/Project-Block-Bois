
//{{BLOCK(stage2)

//======================================================================
//
//	stage2, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-03-08, 15:53:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE2_H
#define GRIT_STAGE2_H

#define stage2TilesLen 64
extern const unsigned short stage2Tiles[32];

#define stage2MapLen 2048
extern const unsigned short stage2Map[1024];

#define stage2PalLen 512
extern const unsigned short stage2Pal[256];

#endif // GRIT_STAGE2_H

//}}BLOCK(stage2)
