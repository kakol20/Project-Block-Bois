
//{{BLOCK(tutorial_walls)

//======================================================================
//
//	tutorial_walls, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 64 + 2048 = 2624
//
//	Time-stamp: 2018-04-12, 16:59:00
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TUTORIAL_WALLS_H
#define GRIT_TUTORIAL_WALLS_H

#define tutorial_wallsTilesLen 64
extern const unsigned short tutorial_wallsTiles[32];

#define tutorial_wallsMapLen 2048
extern const unsigned short tutorial_wallsMap[1024];

#define tutorial_wallsPalLen 512
extern const unsigned short tutorial_wallsPal[256];

#endif // GRIT_TUTORIAL_WALLS_H

//}}BLOCK(tutorial_walls)
