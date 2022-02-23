#ifndef ZSDL_H
#define ZSDL_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_Mixer.h"
#include "assets.h"
#include "zmath.h"

typedef struct ZSDL_Cursor ZSDL_Cursor;
typedef struct Viewport Viewport;
typedef struct Camera Camera;
typedef struct Controller Controller;
typedef struct Assets Assets;

typedef struct Dot Dot;
typedef struct Bubble Bubble;
typedef struct Particles Particles;
typedef struct ZSDL_Font zFont;


/*vvvvvvvvvvvvvvvvvvvvvvvvvv VIEWPORT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
// 1/5th of full HD(1920x1080) =  384 x 216

#define ZSDL_INTERNAL_WIDTH 384
#define ZSDL_INTERNAL_HEIGHT 216
#define ZSDL_INTERNAL_HALFWIDTH 192
#define ZSDL_INTERNAL_HALFHEIGHT 108

#define ZSDL_RENDERLAYERS_MAX 5
typedef enum
{
    ZSDL_RENDERLAYER_BACKGROUND,
    ZSDL_RENDERLAYER_ENTITIES,
    ZSDL_RENDERLAYER_FOREGROUND,
    ZSDL_RENDERLAYER_UI,
    ZSDL_RENDERLAYER_POST_PROCESS,
}ZSDL_RENDERLAYER;


// viewport settings u64 layout
// bits for single flags and bytes for numbers
// bitpos   63                                                                            0
// bytepos  ____7____ ____6____ ____5____ ____4____ ____3____ ____2____ ____1____ ____0____
//          0000'0000 0000'0000 0000'0000 0000'0000 0000'0000 0000'0000 0000'0000 0000'0000
// layout                       FADECOLOR FADEALPHA PIXLSCALE CURSOR ID BITFLAGSB BITFLAGSA 

#define ZSDL_SETTINGS_BIT_SCANLINEFILTER 8
#define ZSDL_SETTINGS_BYTE_BITFLAGS_A 0
#define ZSDL_SETTINGS_BYTE_BITFLAGS_B 1
#define ZSDL_SETTINGS_BYTE_ACTIVE_CURSOR 2
#define ZSDL_SETTINGS_BYTE_PIXELSCALE 3
#define ZSDL_SETTINGS_BYTE_FADE_ALPHA 4
#define ZSDL_SETTINGS_BYTE_FADE_COLOR 5
typedef struct Viewport
{
    SDL_Window*     window;
    SDL_Surface*    surface;
    SDL_Renderer*   renderer;
    SDL_Texture*    render_layer[ZSDL_RENDERLAYERS_MAX];
    Camera*         camera;
    SDL_Rect        screen;
    u64             settings;
} Viewport;

b8 SetupSDL();
Viewport* CreateViewport(const char* window_title);
void FreeViewport(Viewport* viewport);
void ComputePixelScale(Viewport* viewport);
void CalculateScreen(Viewport* viewport);
void ToggleFullscreen(Viewport* viewport);


/*^^^^^^^^^^^^^^^^^^^^^^^^^^ VIEWPORT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv SPEAKER vvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ SPEAKER ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv FONT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define ZFONT_DEFAULT_MAX_COL 16
#define ZFONT_DEFAULT_MAX_ROW 6
#define ZFONT_ASCII_OFFSET 32
typedef struct ZSDL_Font
{
    SDL_Texture* glyphs;
    i2 siz;
    i2 spacing;
} zFont;

void DrawTextWorld(Viewport* viewport, zFont* font, SDL_Color color, r2 pos, r32 depth, const char* text);
void DrawTextScreenCentered(Viewport* viewport, zFont* font, SDL_Color color, SDL_Rect dst, const char* text);
void DrawTextScreen(Viewport* viewport, zFont* font, SDL_Color color, i2 loc, const char* text);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ FONT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv ASSETBANK vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define ASSETBANK_TEXTURES_MAX 16
#define ASSETBANK_SOUNDS_MAX 8
#define ASSETBANK_MUSIC_MAX 2
#define ASSETBANK_CURSORS_MAX 7
#define ASSETBANK_SURFACES_MAX 6
#define ASSETBANK_FONTS_MAX 2
#define ASSETBANK_STRINGS_MAX 5

typedef struct Assets
{
	SDL_Texture* tex[ASSETBANK_TEXTURES_MAX];
	Mix_Chunk* sfx[ASSETBANK_SOUNDS_MAX];
    Mix_Music* mus[ASSETBANK_MUSIC_MAX];
	ZSDL_Cursor* cur[ASSETBANK_CURSORS_MAX];
	SDL_Surface* sur[ASSETBANK_SURFACES_MAX];
	zFont* fon[ASSETBANK_FONTS_MAX];
	char* str[ASSETBANK_STRINGS_MAX];
} Assets;

Assets* CreateAssets(Viewport* viewport);
void FreeAssets(Assets* assets);
void LoadSound(Assets* assets, i32 identifier, const char* path);
void LoadMusic(Assets* assets, i32 identifier, const char* path);
void LoadSurface(Assets* assets, i32 identifier, const char* path);
void LoadString(Assets* assets, i32 identifier, const char* path);
void GenerateString(Assets* assets, i32 identifier, const char* string);
void LoadTexture(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path);
void LoadCursor(Assets* assets, i32 identifier, const char* path);
void LoadFont(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path);
void MixSFX();
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ ASSETBANK ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv INPUT CONTROLLER vvvvvvvvvvvvvvvvvvvvvvvvvv*/
// ACTION MASKS
//  usage: if (Actions.player & ACTION(A_PLR_JMP) ) -> doJumpAction
//  store current actions in lower 32 bits and previous actions in upper 32 bits
#define ACTION(X) ((u64)1 << ((X)-1))
#define ACTION_PRE(X) ((u64)1 << (((X)-1) + 32))

#define A_QUIT 1
#define A_PLAY 2
#define A_EDIT 3
#define A_DBUG 4
#define A_JUMP 5
#define A_WHLU 6 // mouse wheel up
#define A_WHLD 7
#define A_SHFT 8
#define A_MB_L 9
#define A_MB_R 10
#define A_MOVL 11
#define A_MOVR 12
#define A_MOVU 13
#define A_MOVD 14
#define A_FSCR 15 // fullscreen
#define A_ESC 16
#define A_RSIZ 17 // resize window
#define A_ONE 18 //numbers, usually action or hotbar
#define A_TWO 19 //numbers, usually action or hotbar
#define A_THREE 20 //numbers, usually action or hotbar
#define A_FOUR 21 //numbers, usually action or hotbar
#define A_TAB 22

typedef struct Controller
{
    u64 actions;
	i2  move_vector;
	i2  directional_vector;
	i2  mouse_location;
} Controller;

Controller* CreateController();
void FreeController(Controller* controller);

void CollectInput(Controller* c);
i2 MouseLocation(Controller* c, Viewport* viewport);
b8 ActionPressed(Controller* c, u64 action);
b8 ActionReleased(Controller* c, u64 action);
b8 ActionHeld(Controller* c, u64 action);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INPUT CONTROLLER ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv CAMERA vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define ZSDL_CAMERA_MAX_ZOOM 8.f
#define ZSDL_CAMERA_MIN_ZOOM 0.25f
typedef struct Camera
{
    r2 pos;
    r2 aim;
    i2 off;
    r32 zoom;
} Camera;

Camera* CreateCamera(r2 pos);
void FreeCamera(Camera* camera);


/*^^^^^^^^^^^^^^^^^^^^^^^^^^ CAMERA ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv PARTICLES vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define DOTS_MAX 200
typedef struct Dot
{
	u16 lifetime;
	u16 current_life;
	u8 r;
	u8 b;
	u8 g;
	u8 a;
	u8 r_0;
	u8 g_0;
	u8 b_0;
	u8 a_0;
	u8 r_1;
	u8 g_1;
	u8 b_1;
	u8 a_1;
	r2 acc;
	r2 vel;
	r2 pos;
    r32 depth;
} Dot;

#define BUBBLES_MAX 128
typedef struct Bubble
{
	u16 lifetime;
	u16 current_life;
	u8 r;
	u8 b;
	u8 g;
	u8 a;
	u8 r_0;
	u8 g_0;
	u8 b_0;
	u8 a_0;
	u8 r_1;
	u8 g_1;
	u8 b_1;
	u8 a_1;
	r2 acc;
	r2 vel;
	r2 pos;
    r32 rad;
    r32 rad_0;
    r32 rad_1;
    r32 depth;
} Bubble;


typedef struct Particles
{
	Dot dots[DOTS_MAX];
	Bubble bubbles[BUBBLES_MAX];
} Particles;

Particles* InitParticles();
b8 SpawnBubble(Particles* p, u16 lifetime, r2 pos, r2 vel, r2 acc, r32 depth, r32 initial_radius, r32 final_radius, SDL_Color initial_color, SDL_Color final_color);
b8 SpawnDot(Particles* p, u16 lifetime, r2 pos, r2 vel, r2 acc, r32 depth, SDL_Color initial_color, SDL_Color final_color);
void TickParticles(Particles* p, u32 t, r32 dt);
void DrawParticles(Viewport* viewport, u32 t, Particles* p);
void FreeParticles(Particles* p);

// Dots* initDots();
// void tickDots(Dots* dots, u32 t, r32 dt);
// void DrawDots(Viewport* viewport, u32 t, Dots* dots);
// void FreeDots(Dots* dots);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ PARTICLES ^^^^^^^^^^^^^^^^^^^^^^^^^^*/





/*vvvvvvvvvvvvvvvvvvvvvvvvvv CURSOR vvvvvvvvvvvvvvvvvvvvvvvvvv*/

#define ZSDL_CURSOR_BASE_SIZE 15
typedef struct ZSDL_Cursor
{
	SDL_Surface* source_bitmap;
	SDL_Cursor* cursor;
	i2 hotspot;
} ZSDL_Cursor;

void SetCursor(Viewport* viewport, Assets* assets, u8 new_cursor);
void RefreshCursors(Viewport* viewport, Assets* assets);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ CURSOR ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv GUI vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define BUTTON_STATE_MASK_CURR	0x0f
#define BUTTON_STATE_MASK_PREV	0xf0
#define BUTTON_STATE_POS_NOW	0
#define BUTTON_STATE_POS_OLD	1
#define BUTTON_STATE_MAX        6
#define BUTTON_ARROW_SIZE 16
#define BUTTON_NINSLICE_DIM 16
typedef enum
{
    BUTTON_STATE_INACTIVE   = 0x00,
    BUTTON_STATE_ACTIVE     = 0x01,
    BUTTON_STATE_HOVERED    = 0x02,
    BUTTON_STATE_PRESSED    = 0x03,
    BUTTON_STATE_HELD       = 0x04,
    BUTTON_STATE_RELEASED   = 0x05
} E_BUTTON_STATE;
typedef struct Button
{
	i2			src_loc;
    i2		    dst_loc;
    i2		    dst_siz;
    char*       txt;
	u32 		slice_dim;
	i32			txt_offset_y_current;
	i32			txt_offset_y_normal;
	i32			txt_offset_y_hovered;
	i32			txt_offset_y_pressed;
    u8          state;
} Button;

typedef enum
{
	BTN_PLAY = 0,
	BTN_OPTS,
	BTN_QUIT,
	BTN_OPTS_VIDEO = 0,
	BTN_OPTS_AUDIO,
	BTN_OPTS_INPUT,
	BTN_OPTS_RETRN,
	BTN_OPTS_VIDEO_FSCREEN = 0,
	BTN_OPTS_AUDIO_MAIN_VOLUME = 0,
	BTN_OPTS_INPUT_REBIND = 0,
} E_Button_Name;
#define MAX_BUTTONS 10

typedef enum
{
	MENU_TITLE,
	MENU_OPTIONS,
	MENU_OPTIONS_VIDEO,
	MENU_OPTIONS_AUDIO,
	MENU_OPTIONS_INPUT,
	MENU_QUIT_PROMPT,
} E_Menu_Name;

#define MAX_MENUS 5
typedef struct Menu
{
	Button* buttons;
    u32 num_buttons;
	//TODO: the only other "widget" we need is a general, placeable textbox for headers and explanations. add this
	//TODO: a textbox type widget can also be used for tooltips if implemented correctly
} Menu;




Menu CreateMenu(const char* config_section);
Button AddButton(i2 src_loc, u32 slice_dim, r2 margins_x, r2 margins_y, const char* text, i8 txt_offset_y_normal, i8 txt_offset_y_hovered, i8 txt_offset_y_pressed);
i32 TickMenu(Menu menu, i2 mouse_location, Controller* controller);
void DrawMenu(Menu menu, Viewport* viewport, Assets* assets);
void ToggleMenu(Menu* menu, b8 enable);
void FreeMenus(Menu* menus);

char* ButtonStateName(E_BUTTON_STATE state);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ GUI ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv RENDER SUPPORT FUNCTIONS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
void BlurRenderlayer(Viewport* viewport, ZSDL_RENDERLAYER renderlayer, i32 amount, i32 opacity);
void CleanRenderTargets(Viewport* viewport);
void FinalizeRenderAndPresent(Viewport* viewport);

void DrawNumber(Viewport* viewport, SDL_Texture* texture, u32 number, i2 size_src, i2 size_dst, i2 location, u32 max_digits);
void ZSDL_RenderDrawCircle(Viewport* viewport, u32 radius, i2 center);
void DrawNineSliced(Viewport* viewport, struct SDL_Texture* source_texture, i2 src_loc, i2 dst_loc, i2 dst_siz, i32 slice_dimensions);

i2 PosToCam(r2 pos, r32 depth, Viewport* viewport);
r2 CamToPos(i2 cam, Viewport* viewport);

#define COLOR(r,g,b,a) (SDL_Color){r, g, b, a}
#define COLOR_RED COLOR(0xff, 0x00, 0x00, 0xff)
#define COLOR_GREEN COLOR(0x00, 0xff, 0x00, 0xff)
#define COLOR_BLUE COLOR(0x00, 0x00, 0xff, 0xff)
#define COLOR_YELLOW COLOR(0xff, 0xff, 0x00, 0xff)
#define COLOR_CYAN COLOR(0x00, 0xff, 0xff, 0xff)
#define COLOR_PURPLE COLOR(0xff, 0x00, 0xff, 0xff)
#define COLOR_BLACK COLOR(0x00, 0x00, 0x00, 0xff)
#define COLOR_WHITE COLOR(0xff, 0xff, 0xff, 0xff)
#define COLOR_WHITE_TRANSPARENT COLOR(0xff, 0xff, 0xff, 0x00)
#define COLOR_BLACK_TRANSPARENT COLOR(0xff, 0xff, 0xff, 0x00)
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ RENDER SUPPORT FUNCTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv ANALYSIS FUNCTIONS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
void DPrintMouseLoc(Controller* controller, Viewport* viewport, Assets* assets);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ ANAALYSIS FUNCTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#endif // ZSDL_H
