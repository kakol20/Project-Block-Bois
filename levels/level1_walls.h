
//{{BLOCK(level1_walls)

//======================================================================
//
//	level1_walls, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-12, 15:18:06
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_LEVEL1_WALLS_H
#define GRIT_LEVEL1_WALLS_H

#define level1_wallsTilesLen 64
extern const unsigned short level1_wallsTiles[32];

#define level1_wallsMapLen 2048
extern const unsigned short level1_wallsMap[1024];

#define level1_wallsPalLen 512
extern const unsigned short level1_wallsPal[256];

#endif // GRIT_LEVEL1_WALLS_H

//}}BLOCK(level1_walls)
