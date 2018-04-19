
//{{BLOCK(_StageWall)

//======================================================================
//
//	_StageWall, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-19, 15:44:17
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT__STAGEWALL_H
#define GRIT__STAGEWALL_H

#define _StageWallTilesLen 64
extern const unsigned short _StageWallTiles[32];

#define _StageWallMapLen 2048
extern const unsigned short _StageWallMap[1024];

#define _StageWallPalLen 512
extern const unsigned short _StageWallPal[256];

#endif // GRIT__STAGEWALL_H

//}}BLOCK(_StageWall)
