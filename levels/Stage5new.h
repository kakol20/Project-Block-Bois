
//{{BLOCK(Stage5new)

//======================================================================
//
//	Stage5new, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-05-03, 14:52:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STAGE5NEW_H
#define GRIT_STAGE5NEW_H

#define Stage5newTilesLen 64
extern const unsigned short Stage5newTiles[32];

#define Stage5newMapLen 2048
extern const unsigned short Stage5newMap[1024];

#define Stage5newPalLen 512
extern const unsigned short Stage5newPal[256];

#endif // GRIT_STAGE5NEW_H

//}}BLOCK(Stage5new)
