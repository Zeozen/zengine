#include <stdio.h>
#include "render.h"
#include "zengine.h"



/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderInit    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;
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
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderEvent   (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderLose    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderGoal    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderEdit    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderExit    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;
}

// /*-----------------------------------------------------------*/
// /*-----------------------------------------------------------*/
// void RenderMain
// (
//     u32 t_r,
//     Viewport* viewport, 
//     Game* game, 
//     Controller* controller, 
//     Particles* particles,
//     Assets* assets,
//     Menu* menu
// )/*-----------------------------------------------------------*/
// {/*-----------------------------------------------------------*/

//      SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);

// //draw world
//     SDL_SetRenderDrawColor(viewport->renderer, 0x22, 0x15, 0x15, 0xff);
//     SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, 0xff);
//     SDL_RenderFillRect(viewport->renderer, NULL);

//     i2 origo_to_screen = PosToCam(ZERO_R2, 1.f, viewport);
//     SDL_SetRenderDrawColor(viewport->renderer, 0xcc, 0xaa, 0xaa, 0x33);
//     SDL_RenderDrawLine(viewport->renderer, origo_to_screen.x, 0, origo_to_screen.x, ZSDL_INTERNAL_HEIGHT);
//     SDL_RenderDrawLine(viewport->renderer, 0, origo_to_screen.y, ZSDL_INTERNAL_WIDTH, origo_to_screen.y);
    
// //draw home
//     SDL_SetRenderDrawColor(viewport->renderer, 0xbb, 0xbb, 0xbb, 0xbb);
//     ZSDL_RenderDrawCircle(viewport, 32.f * viewport->camera->zoom, PosToCam(ZERO_R2, 1.f, viewport));

//     SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);

//     SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);

//     SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
//     DrawMenu(menu[MENU_TITLE], viewport, assets);

//     //DrawTextWorld(viewport, assets->fon[FONT_ID_ZSYS], COLOR_WHITE, ZERO_R2, 8.f, "THERE");
//     //DrawTextWorld(viewport, assets->fon[FONT_ID_ZSYS], COLOR_WHITE, ZERO_R2, 4.f, "HELLO");
 
// }


// /*-----------------------------------------------------------*/
// /*-----------------------------------------------------------*/
// void RenderOpts
// (
//     u32 t_r,
//     Viewport* viewport, 
//     Game* game, 
//     Controller* controller, 
//     Particles* particles,
//     Assets* assets,
//     Menu* menu
// )/*-----------------------------------------------------------*/
// {/*-----------------------------------------------------------*/

//     SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
//     DrawMenu(menu[MENU_OPTIONS], viewport, assets);
//     DrawMenu(menu[MENU_OPTIONS_VIDEO], viewport, assets);
//     DrawMenu(menu[MENU_OPTIONS_AUDIO], viewport, assets);
//     DrawMenu(menu[MENU_OPTIONS_INPUT], viewport, assets);
// }



// /*-----------------------------------------------------------*/
// /*-----------------------------------------------------------*/
// void RenderPlay
// (
//     u32 t_r,
//     Viewport* viewport, 
//     Game* game, 
//     Controller* controller, 
//     Particles* particles,
//     Assets* assets,
//     Menu* menu
// )/*-----------------------------------------------------------*/
// {/*-----------------------------------------------------------*/


//     #ifdef DBUG_MOUSE
//     char txt_mraw[50];
//     char txt_mpos[50];
//     char txt_mcam[50];
//     i2 mraw = MouseLocation(controller, viewport);
//     r2 mpos = CamToPos(mraw, viewport);
//     i2 mcam = PosToCam(mpos, viewport);
//     sprintf(txt_mraw, "mraw: (%d, %d)", mraw.x, mraw.y);
//     sprintf(txt_mpos, "mpos: (%f, %f)", mpos.x, mpos.y);
//     sprintf(txt_mcam, "mcam: (%d, %d)", mcam.x, mcam.y);
//     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(3, 3), txt_mraw);
//     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(3, 3 + assets->fon[0]->siz.y), txt_mpos);
//     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(3, 3+ assets->fon[0]->siz.y*2), txt_mcam);
//     #endif
// }

// /*-----------------------------------------------------------*/
// /*-----------------------------------------------------------*/
// void RenderLose
// (
//     u32 t_r,
//     Viewport* viewport, 
//     Game* game, 
//     Controller* controller, 
//     Particles* particles,
//     Assets* assets,
//     Menu* menu
// )/*-----------------------------------------------------------*/
// {/*-----------------------------------------------------------*/

// }

