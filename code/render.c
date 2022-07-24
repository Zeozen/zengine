#include <stdio.h>
#include "render.h"
#include "zengine.h"



/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderInit    (u32 t, r32 a, void* engine)
{
    //zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderMain    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;


    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);

//draw world
    SDL_SetRenderDrawColor(z->viewport->renderer, 0x22, 0x15, 0x15, 0xff);
    SDL_SetRenderDrawColor(z->viewport->renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderFillRect(z->viewport->renderer, NULL);

    i2 origo_to_screen = PosToCam(ZERO_R2, 1.f, z->viewport);
    SDL_SetRenderDrawColor(z->viewport->renderer, 0xcc, 0xaa, 0xaa, 0x33);
    SDL_RenderDrawLine(z->viewport->renderer, origo_to_screen.x, 0, origo_to_screen.x, ZSDL_INTERNAL_HEIGHT);
    SDL_RenderDrawLine(z->viewport->renderer, 0, origo_to_screen.y, ZSDL_INTERNAL_WIDTH, origo_to_screen.y);
    
//draw home
    SDL_SetRenderDrawColor(z->viewport->renderer, 0xbb, 0xbb, 0xbb, 0xbb);
    ZSDL_RenderDrawCircle(z->viewport, 32.f * z->viewport->camera->zoom, PosToCam(ZERO_R2, 1.f, z->viewport));

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    DrawMenu(z->menus[MENU_TITLE], z->viewport, z->assets);

//draw player cursors
    for (i32 i = 0; i < MAX_PLAYERS; i++)
    {
        if (z->input->pcon[i]->active)
        {
            SDL_Rect src = {0, 0, 8, 8};
            SDL_Rect dst = {z->input->pcon[i]->cursor_loc.x, z->input->pcon[i]->cursor_loc.y, 8, 8};
            SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_PLAYER_CURSOR], &src, &dst);
            char nav_val[50];
            sprintf(nav_val, "nav: (%f, %f)", z->input->pcon[i]->nav.x, z->input->pcon[i]->nav.y);
            DrawTextScreen(z->viewport, z->assets->fon[FONT_ID_ZSYS], COLOR_WHITE, ZERO_I2, nav_val);
        }
    }


}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderOptions (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    DrawMenu(z->menus[MENU_OPTIONS], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_OPTIONS_VIDEO], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_OPTIONS_AUDIO], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_OPTIONS_INPUT], z->viewport, z->assets);
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderPlay    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

    //draw our testing cube mesh
    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
    SDL_SetRenderDrawColor(z->viewport->renderer, 255, 255, 255, 255);

    //rotation matricies
    mat4x4 mat_rot_z, mat_rot_x;

    static r32 theta = 0.f;
    theta += 0.05f;

    for (i32 a = 0; a < 4; a++)
    {
        for (i32 b = 0; b < 4; b++)
        {
            mat_rot_z.m[a][b] = 0.f;
            mat_rot_x.m[a][b] = 0.f;
        }
    }

    mat_rot_z.m[0][0] = cosf(theta);
    mat_rot_z.m[0][1] = sinf(theta);
    mat_rot_z.m[1][0] = -sinf(theta);
    mat_rot_z.m[1][1] = cosf(theta);
    mat_rot_z.m[2][2] = 1.f;
    mat_rot_z.m[3][3] = 1.f;

    mat_rot_x.m[0][0] = 1.f;
    mat_rot_x.m[1][1] = cosf(theta * 0.5f);
    mat_rot_x.m[1][2] = sinf(theta * 0.5f);
    mat_rot_x.m[2][1] = -sinf(theta * 0.5f);
    mat_rot_x.m[2][2] = cosf(theta * 0.5f);
    mat_rot_x.m[3][3] = 1.f;

    for (i32 i = 0; i < z->assets->msh[0]->num; i++)//for all tris in mesh
    {
        tri projected_tri, translated_tri, rotated_tri_z, rotated_tri_zx;
        

        //rotation
        for (i32 r = 0; r < 3; r++)
            MultiplyMatrixVector(&z->assets->msh[0]->tris[i].vert[r], &rotated_tri_z.vert[r], &mat_rot_z);
        for (i32 r = 0; r < 3; r++)
            MultiplyMatrixVector(&rotated_tri_z.vert[r], &rotated_tri_zx.vert[r], &mat_rot_x);

        //translate
        translated_tri = rotated_tri_zx;
        for (i32 t = 0; t < 3; t++)
        {
            translated_tri.vert[t].z = rotated_tri_zx.vert[t].z + 3.f;
        }

        //projection
        for (i32 v = 0; v < 3; v++)
            MultiplyMatrixVector(&translated_tri.vert[v], &projected_tri.vert[v], z->viewport->projection);

        //scale
        for (i32 s = 0; s < 3; s++)
        {
            projected_tri.vert[s].x += 1.f;
            projected_tri.vert[s].y += 1.f;
        }
        for (i32 s = 0; s < 3; s++)
        {
            projected_tri.vert[s].x *= ZSDL_INTERNAL_HALFWIDTH;
            projected_tri.vert[s].y *= ZSDL_INTERNAL_HALFHEIGHT;
        }

        for (i32 d = 0; d < 3; d++)
            SDL_RenderDrawLineF(z->viewport->renderer, projected_tri.vert[d].x, projected_tri.vert[d].y, projected_tri.vert[(d+1)%3].x, projected_tri.vert[(d+1)%3].y);
    }
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderEvent   (u32 t, r32 a, void* engine)
{
    //zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderLose    (u32 t, r32 a, void* engine)
{
    //zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderGoal    (u32 t, r32 a, void* engine)
{
    //zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderEdit    (u32 t, r32 a, void* engine)
{
    //zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderExit    (u32 t, r32 a, void* engine)
{
    //zEngine* z = (zEngine*)engine;
}

