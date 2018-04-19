
//{{BLOCK(_StageFloor)

//======================================================================
//
//	_StageFloor, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-19, 15:44:36
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT__STAGEFLOOR_H
#define GRIT__STAGEFLOOR_H

#define _StageFloorTilesLen 64
extern const unsigned short _StageFloorTiles[32];

#define _StageFloorMapLen 2048
extern const unsigned short _StageFloorMap[1024];

#define _StageFloorPalLen 512
extern const unsigned short _StageFloorPal[256];

#endif // GRIT__STAGEFLOOR_H

//}}BLOCK(_StageFloor)
