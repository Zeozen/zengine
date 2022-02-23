#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zmath.h"
#include "zgrid.h"

zGrid* CreateGrid(u16 width, u16 height, r2 origin)
{
	zGrid* new_grid = malloc(sizeof(zGrid));
	memset(new_grid, 0, sizeof(zGrid));
	new_grid->cell = calloc(width*height, sizeof(Cell));

	new_grid->width 	= width;
	new_grid->height 	= height;
	new_grid->origin 	= origin;

//deserialize
	FILE* flvl = fopen("./assets/design/level.bin", "rb");
	u8* buf = malloc(sizeof(u8)*8);

	for (i32 i = 0; i < width * height; i++)
	{
		fread(buf, sizeof(u8), 8, flvl);
		new_grid->cell[i].collision = buf[0];
		new_grid->cell[i].type = buf[1];
		new_grid->cell[i].id = buf[2];
		new_grid->cell[i].sprite_bg = buf[3];
		new_grid->cell[i].sprite_mg = buf[4];
		new_grid->cell[i].sprite_fg = buf[5];
		new_grid->cell[i].unused_0 = buf[6];
		new_grid->cell[i].unused_1 = buf[7];
	}

	fclose(flvl);
	free(buf);


/* FLUSHING */
	// for (i32 y = 0; y < height; y++)
	// {
	// 	for (i32 x = 0; x < width; x++)
	// 	{
	// 		if (x == 0 || x == width - 1 || y == 0 || y == height -1)
	// 		{
	// 			i32 i = x + y * width;
	// 			new_grid->cell[i].collision = 1;
	// 			new_grid->cell[i].type = 1;
	// 			new_grid->cell[i].sprite_mg = MAKE8FROM4(1, 0);
	// 		}
	// 		else
	// 		{
	// 			i32 i = x + y * width;
	// 			new_grid->cell[i].collision = 0;
	// 			new_grid->cell[i].type = 0;
	// 			new_grid->cell[i].sprite_mg = 0;

	// 		}
	// 	}
	// }
	// 	//save edits
	// FILE* flushlevel = fopen("./assets/design/level.bin", "wb");
	// u8* flush_buf = malloc(sizeof(u8)*8);

	// for (i32 i = 0; i < new_grid->width * new_grid->height; i++)
	// {
	// 	flush_buf[0] = new_grid->cell[i].collision;
	// 	flush_buf[1] = new_grid->cell[i].type;
	// 	flush_buf[2] = new_grid->cell[i].id;
	// 	flush_buf[3] = new_grid->cell[i].sprite_bg;
	// 	flush_buf[4] = new_grid->cell[i].sprite_mg;
	// 	flush_buf[5] = new_grid->cell[i].sprite_fg;
	// 	flush_buf[6] = new_grid->cell[i].unused_0;
	// 	flush_buf[7] = new_grid->cell[i].unused_1;
	// 	fwrite(flush_buf, sizeof(u8), 8, flushlevel);
	// }

	// fclose(flushlevel);
	// free(flush_buf);
/* FLUSHING END*/



	return new_grid;
}

void FreeGrid(zGrid* grid)
{



	if (grid != NULL)
	{
		free(grid->cell);
		grid->cell = NULL;
		free(grid);
		grid = NULL;
	}
}

void DrawGrid(zGrid* grid, Viewport* viewport, Assets* assets, u32 anim)
{
// find cels to draw //TODO zooming
	r2 cel_0_pos = sub_r2(viewport->camera->pos, make_r2(ZSDL_INTERNAL_HALFWIDTH, ZSDL_INTERNAL_HALFHEIGHT));
	r32 min_x = MaxR32(cel_0_pos.x, grid->origin.x);
	r32 min_y = MaxR32(cel_0_pos.y, grid->origin.y);
	i2 cel_0 = PosToCel(make_r2(min_x, min_y), grid);
	i2 cel_1 = i2_add_n(add_i2(cel_0, make_i2(ZSDL_INTERNAL_WIDTH/WORLD_UNIT, ZSDL_INTERNAL_HEIGHT/WORLD_UNIT)), 1);
	i2 min_1 = make_i2(MinI32(cel_1.x, grid->width), MinI32(cel_1.y, grid->height));



//draw grid background layer
	
	//i2 origin_cam = PosToCam(grid->origin, 1.f, viewport);
	// SDL_Rect whole_grid = {origin_cam.x, origin_cam.y, grid->width*WORLD_UNIT, grid->height*WORLD_UNIT};
	// SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0x00, 0x00, 0xff);
	// SDL_RenderFillRect(viewport->renderer, &whole_grid);
//draw grid gameplay layer
	SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
	for (i32 v = cel_0.y; v < min_1.y; v++)
	{
		for (i32 u = cel_0.x; u < min_1.x; u++)
		{
			i2 cel = make_i2(u, v);
			SDL_Rect src;
			if (ValidateCel(cel, grid))
			{
				i32 idx = CelToIdx(cel, grid);
				r2 cel_pos = CelToPos(cel, grid);
				i2 cel_cam = PosToCam(cel_pos, 1.f, viewport);
				SDL_Rect dst = {cel_cam.x, cel_cam.y, WORLD_UNIT, WORLD_UNIT};

				if (grid->cell[idx].sprite_bg)
				{
					SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
					src = CelSpriteSource(idx, grid, SPRITELAYER_BG);
					src.x = anim * WORLD_UNIT;
					SDL_RenderCopy(viewport->renderer, assets->tex[T_TILE_ATLAS], &src, &dst);
				}
				
					SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
					src = CelSpriteSource(idx, grid, SPRITELAYER_MG);
					src.x = anim * WORLD_UNIT;
					SDL_RenderCopy(viewport->renderer, assets->tex[T_TILE_ATLAS], &src, &dst);

			}
		}
	}
	// for (i32 i = 0; i < grid->width*grid->height; i++)
	// {
	// 	i2 cel = IdxToCel(i, grid);
	// 	i32 idx = CelToIdx(cel, grid);
	// 	SDL_Rect src = CelSpriteSource(idx, grid, SPRITELAYER_MG);
	// 	r2 cel_pos = CelToPos(cel, grid);
	// 	i2 cel_cam = PosToCam(cel_pos, 1.f, viewport);
	// 	SDL_Rect dst = {cel_cam.x, cel_cam.y, WORLD_UNIT, WORLD_UNIT};

	// 	SDL_RenderCopy(viewport->renderer, assets->tex[T_TILE_ATLAS], &src, &dst);
	// }

// //draw grid foreground layer
// 	SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);
}


u8 ValidateCel(i2 cel, zGrid* grid)
{
	if (cel.x >= 0 && cel.x < grid->width && cel.y >= 0 && cel.y < grid->height)
		return 1;
	else
		return 0;
}

u32 CelToIdx(i2 cel, zGrid* grid)
{
	//if (ValidateCel(cel, grid))
		return cel.x + cel.y * grid->width;
	//else
	//	return 0;
}

i2 IdxToCel( u32 index, zGrid* grid)
{
	//if (index < grid->width * grid->height)
	//{
		return make_i2(index % grid->width, index / grid->width);
//	//}
//	//else
//	//{
//	//	return make_i2(0, 0);
	//}
}


r2 CelToPos(i2 cel, zGrid* grid)
{
	// pos = grid.origin + cel * world unit
	r32 x = grid->origin.x + cel.x * WORLD_UNIT;
	r32 y = grid->origin.y + cel.y * WORLD_UNIT;
	return make_r2(x, y);
	//return sub_r2(i2_to_r2(i2_mul_n(cel, WORLD_UNIT)), grid->origin);
}

i2 PosToCel(r2 pos, zGrid* grid)
{
	// cel = (pos / gridsize) - origin
	r2 normalized = sub_r2(pos, grid->origin);
	i2 pix = r2_to_i2(normalized);
	i2 cel = i2_div_n(pix, WORLD_UNIT);
	//if (ValidateCel(cel, grid))
		return cel;
	//else
	//	return ZERO_I2;
	//i2 cel = i2_div_n(r2_to_i2(pos), WORLD_UNIT);
	//return add_i2(cel, r2_to_i2(grid->origin));
}

u32 PosToIdx( r2 pos, zGrid* grid)
{
	return CelToIdx(PosToCel(pos, grid), grid);
}

r2 IdxToPos( u32 idx, zGrid* grid)
{
	return CelToPos(IdxToCel(idx, grid), grid);
}

SDL_Rect CelSpriteSource(i32 idx, zGrid* grid, i32 layer)
{
	SDL_Rect src = {0, 0, WORLD_UNIT, WORLD_UNIT};
	switch (layer)
	{
	case SPRITELAYER_BG:
		src.y = GET4IN8(grid->cell[idx].sprite_bg, BITPOS_SPRITE_ROW) * WORLD_UNIT;
		break;
	case SPRITELAYER_MG:
		src.y = GET4IN8(grid->cell[idx].sprite_mg, BITPOS_SPRITE_ROW) * WORLD_UNIT;
		break;
	case SPRITELAYER_FG:
		// row = GET4IN8(grid->cell[idx].sprite_fg, BITPOS_SPRITE_ROW);
		// col = GET4IN8(grid->cell[idx].sprite_fg, BITPOS_SPRITE_COL);
		break;
	default:
		break;
	}
	return src;
}

b8 CelSolid(i2 cel, zGrid* grid)
{
	i32 idx = CelToIdx(cel, grid);
	return (grid->cell[idx].collision);
}

b8 TraceMove(zGrid* world, struct Signed_Vector_2 scale, struct Real_Vector_2 ray_beg, struct Real_Vector_2 ray_end, struct Real_Vector_2 *hit_loc, struct Real_Vector_2 *hit_norm, b8 collision_mask)
{
	i2 starting_cel = PosToCel(ray_beg, world);
	i2 ending_cel = PosToCel(ray_end, world);
	i32 x_0, x_1, y_0, y_1;
	r2 candidate_point;
	r2 candidate_point_final = ray_end;
	r2 candidate_normal;
	r2 candidate_normal_final = ZERO_R2;
	b8 result = 0;
		x_0 = MinI32(starting_cel.x, ending_cel.x);
		x_1 = MaxI32(starting_cel.x, ending_cel.x);
		y_0 = MinI32(starting_cel.y, ending_cel.y);
		y_1 = MaxI32(starting_cel.y, ending_cel.y);
	for (int y = y_0 - 2; y <= y_1 + 2 ; y++)
	{
		for (int x = x_0 - 2 ; x <= x_1 + 2; x++)
		{
			if (CelSolid(make_i2(x, y), world))
			{
				//r2 box_pix = make_r2(x*TILE - (scale.x / 2.f), y*TILE - (scale.y / 2.f));
				r2 box_pix = make_r2(x*WORLD_UNIT - (scale.x / 2.f), y*WORLD_UNIT - (scale.y / 2.f));
				r2_box box = {make_r2(box_pix.x, box_pix.y), make_r2((r32)WORLD_UNIT + (r32)scale.x, (r32)WORLD_UNIT + (r32)scale.y)};
				if (r2_line_intersect_box(ray_beg, ray_end, box, &candidate_point, &candidate_normal) == 1)
				{
					if (len2_r2(sub_r2(candidate_point, ray_beg)) < len2_r2(sub_r2(candidate_point_final, ray_beg)))
					{
						candidate_point_final = candidate_point;
						candidate_normal_final = candidate_normal;
						result = 1;
					}
				}
			}
		}
	}
	
	if (result == 1)
	{
		*hit_loc = candidate_point_final;
		*hit_norm = candidate_normal_final;
		return result;
	}
	else
	{
		*hit_loc = ray_end;
		*hit_norm = ZERO_R2;
		return result;
	}
}