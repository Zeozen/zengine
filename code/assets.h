#ifndef ASSETS_H
#define ASSETS_H

//font
#define FONT_ID_ZSYS 0
#define FONT_PATH_ZSYS "./assets/font/font_zsys.png"

//texture aliases
#define T_TILE_ATLAS 0
#define T_UI_ATLAS 1
#define T_PLAYER_ATLAS 2
#define T_PLAYER_CURSOR 3
//texture paths
#define T_UI_ATLAS_PATH         "./assets/texture/gui.png"
#define T_PLAYER_CURSOR_PATH    "./assets/cursor/cur_plr.png"

//cursors
#define CUR_POINT 0
#define CUR_CLICK 1
#define CUR_HAND 2
#define CUR_GRAB 3 
#define CUR_CROSS 4
//cursorpaths
#define CUR_PATH_POINT "./assets/cursor/cur_zsys_point.png"
#define CUR_PATH_CLICK "./assets/cursor/cur_zsys_click.png"
#define CUR_PATH_HAND "./assets/cursor/cur_zsys_hand.png"
#define CUR_PATH_GRAB "./assets/cursor/cur_zsys_grab.png"
#define CUR_PATH_CROSS "./assets/cursor/cur_zsys_cross.png"

#define MUS_BGM 0

//sound effect aliases
typedef enum
{
    SFX_HOVER,
    SFX_SELECT,
    SFX_TAP,
} SoundEffects;
//paths
#define SFX_PATH_HOVER "./assets/sound/hover.wav"
#define SFX_PATH_SELECT "./assets/sound/select.wav"
#define SFX_PATH_TAP "./assets/sound/tap.wav"


#define GUI_DESIGN_PATH "./assets/design/gui.ini"

#endif //ASSETS_H