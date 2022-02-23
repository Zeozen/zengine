#ifndef ZGRID_H
#define ZGRID_H

#include "zmath.h"
#include "zsdl.h"


#define SPRITELAYER_BG 0
#define SPRITELAYER_MG 1
#define SPRITELAYER_FG 2
typedef struct 
{
    u8 collision;
    u8 type;
    u8 id;
    u8 sprite_bg;
    u8 sprite_mg;
    u8 sprite_fg;
    u8 unused_0;
    u8 unused_1;
} Cell;


typedef struct
{
    u16 width;      //x
    u16 height;     //y
    r2 origin;      //in world space
    Cell* cell;
} zGrid;

zGrid* CreateGrid(u16 width, u16 height, r2 origin);
void DrawGrid(zGrid* grid, Viewport* viewport, Assets* assets, u32 anim);
void FreeGrid(zGrid* grid);

u8 ValidateCel(i2 cel, zGrid* grid);
i2  IdxToCel(u32 idx, zGrid* grid);
u32 CelToIdx(i2  cel, zGrid* grid);

i2  PosToCel(r2  pos, zGrid* grid);
r2  CelToPos(i2  cel, zGrid* grid);
r2  IdxToPos(u32 idx, zGrid* grid);
u32 PosToIdx(r2  pos, zGrid* grid);

b8 CelSolid(i2 cel, zGrid* grid);

SDL_Rect CelSpriteSource(i32 idx, zGrid* grid, i32 layer);

b8 TraceMove(zGrid* world, struct Signed_Vector_2 scale, struct Real_Vector_2 ray_beg, struct Real_Vector_2 ray_end, struct Real_Vector_2 *hit_loc, struct Real_Vector_2 *hit_norm, b8 collision_mask);

#define BITMASK_SPRITE_ROW 0xf0
#define BITMASK_SPRITE_COL 0x0f
#define BITPOS_SPRITE_ROW 1
#define BITPOS_SPRITE_COL 0






#endif //ZGRID_H