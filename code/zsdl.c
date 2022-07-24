#include <string.h>
#include "zsdl.h"
#include "ini.h"

//@TODO: use sdl_log instead of printf maybe? It might be more portable.
b8 SetupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		printf("\nSDL could not initialize! SDL_Error: %s", SDL_GetError());
		return 0;
	}
	else
	{
// init SDL_Image
		int flags	= IMG_INIT_PNG;
		int initted = IMG_Init(flags);
		if ((initted & flags) != flags)
		{
			printf("\nIMG_Init: Failed to init PNG support!");
			printf("\nIMG_Init: %s", IMG_GetError());
		}

//init SDL_Mixer
		Mix_Init(MIX_INIT_OGG);
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
		Mix_AllocateChannels(ASSETBANK_SOUNDS_MAX);
		Mix_ReserveChannels(ASSETBANK_SOUNDS_MAX);
		MixSFX();

//init joysticks

//add n64 support
		SDL_GameControllerAddMapping("030000009b2800000c00000000000000,N64,a:b0,b:b1,y:b6,x:b5,start:b3,leftshoulder:b8,rightshoulder:b9,dpup:b10,dpleft:b12,dpdown:b11,dpright:b13,leftx:a0,lefty:a1,lefttrigger:b2,righttrigger:");
		SDL_GameControllerEventState(SDL_IGNORE);
		

		return 1;
	}
}

Viewport* CreateViewport(const char* window_title)
{
	Viewport* viewport = (Viewport*)malloc(sizeof(Viewport));
	viewport->settings = 0;
	#ifdef __EMSCRIPTEN__
	u8 pixel_size_web = 2;
	viewport->screen = (SDL_Rect){0, 0, ZSDL_INTERNAL_WIDTH*pixel_size_web, ZSDL_INTERNAL_HEIGHT*pixel_size_web};
	SET8IN64(pixel_size_web, &viewport->settings, ZSDL_SETTINGS_BYTE_PIXELSCALE);
	#else
	u8 pixel_size_desktop_default = ZSDL_PIXEL_SIZE_DESKTOP_DEFAULT;
	viewport->screen = (SDL_Rect){0, 0, ZSDL_INTERNAL_WIDTH * pixel_size_desktop_default, ZSDL_INTERNAL_HEIGHT * pixel_size_desktop_default};
	SET8IN64(pixel_size_desktop_default, &viewport->settings, ZSDL_SETTINGS_BYTE_PIXELSCALE);
	#endif
#ifdef DEBUGPRNT
	printf("\nInitialising zSDL viewport...");
#endif
	viewport->window =
		SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(0),
						 viewport->screen.w, viewport->screen.h,
						 SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_GRABBED */);
	if (viewport->window == NULL)
	{
		#ifdef DEBUGPRNT
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		#endif	

		return 0;
	}
	else
	{
		#ifdef DEBUGPRNT
		printf("Window successfully created!\n");
		#endif

		viewport->surface = SDL_GetWindowSurface(viewport->window);
		if (viewport->surface == NULL)
		{
			#ifdef DEBUGPRNT
			printf("window surface could not be created! SDL Error: %s\n", SDL_GetError());
			#endif

			return 0;
		}
		else
		{
			#ifdef DEBUGPRNT
			printf("window surface successfully created!\n");
			#endif

		}
		viewport->renderer = SDL_CreateRenderer(
			viewport->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
		if (viewport->renderer == NULL)
		{
			#ifdef DEBUGPRNT
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			#endif

			return 0;
		}
		else
		{
			printf("renderer successfully created!\n");
			for (i32 i = 0; i < ZSDL_RENDERLAYERS_MAX; i++)
			{
				viewport->render_layer[i] =
					SDL_CreateTexture(viewport->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
									  ZSDL_INTERNAL_WIDTH, ZSDL_INTERNAL_HEIGHT);
				if (viewport->render_layer[i] == NULL)
				{
					printf("\nRender layer %d could not be created! SDL Error: %s", i, SDL_GetError());
					return 0;
				}
				else
				{
					printf("\nRender layer %d successfully created!", i);
					SDL_SetTextureBlendMode(viewport->render_layer[i], SDL_BLENDMODE_BLEND);
				}
			}

			//setup projection matrix
			viewport->projection = malloc(sizeof(mat4x4));
			for (i32 v = 0; v < 4; v++)
			{
				for (i32 u = 0; u < 4; u++)
				{
					viewport->projection->m[u][v] = 0.f;		
				}
			}

			//projection defaults
			r32 near_plane	 = 0.1f;
			r32 far_plane	 = 1000.f;
			r32 fov			 = 90.f;
			r32 aspect_ratio = (r32)ZSDL_INTERNAL_HEIGHT / (r32)ZSDL_INTERNAL_WIDTH;
			r32 fov_rad		 = 1.f / tanf(fov * 0.5f / 180.f * ZPI);
			r32 q			 = far_plane / (far_plane - near_plane);

			viewport->projection->m[0][0] = aspect_ratio * fov_rad;
			viewport->projection->m[1][1] = fov_rad;
			viewport->projection->m[2][2] = q;
			viewport->projection->m[3][2] = -1.f * near_plane * q;
			viewport->projection->m[2][3] = 1;
		}
	}

	if (viewport != NULL)
		printf("\nzSDL viewport initialised!");
	else
		printf("\nzSDL viewport failed to initialize!");
	return viewport;
}



void FreeViewport(Viewport* viewport)
{
	SDL_FreeSurface(viewport->surface);
	for (i32 i = 0; i < ZSDL_RENDERLAYERS_MAX; i++)
		SDL_DestroyTexture(viewport->render_layer[i]);
	SDL_DestroyWindow(viewport->window);
	SDL_DestroyRenderer(viewport->renderer);
	if (viewport->camera != NULL)
		free(viewport->camera);
	if(viewport->projection != NULL)
		free(viewport->projection);
	free(viewport);
	printf("\nViewport freed.");
	viewport = NULL;
}




Assets* CreateAssets(Viewport* viewport)
{
	printf("\nInitializing assets...");
    Assets* assets = malloc(sizeof(Assets));
	memset(assets, 0, sizeof(Assets));

	if (assets == NULL)
		printf("\nFailed to init assets!");
	else
		printf("\nAssets initialized.");
	return assets;
}

void FreeAssets(Assets* assets)
{
	for (i32 i = 0; i < ASSETBANK_TEXTURES_MAX; i++)
	{
		if (assets->tex[i] != NULL)
		{
			SDL_DestroyTexture(assets->tex[i]);
			assets->tex[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_SOUNDS_MAX; i++)
	{
		if (assets->sfx[i] != NULL)
		{
			Mix_FreeChunk(assets->sfx[i]);
			assets->sfx[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_MUSIC_MAX; i++)
	{
		if (assets->mus[i] != NULL)
		{
			Mix_FreeMusic(assets->mus[i]);
			assets->mus[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_CURSORS_MAX; i++)
	{
		if (assets->cur[i] != NULL)
		{
			if (assets->cur[i]->cursor != NULL)
				SDL_FreeCursor(assets->cur[i]->cursor);
			if (assets->cur[i]->source_bitmap != NULL)
				SDL_FreeSurface(assets->cur[i]->source_bitmap);
			free(assets->cur[i]);
			assets->cur[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_SURFACES_MAX; i++)
	{
		if (assets->sur[i] != NULL)
		{
			SDL_FreeSurface(assets->sur[i]);
			assets->sur[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_STRINGS_MAX; i++)
	{
		if (assets->str[i] != NULL)
		{
			free(assets->str[i]);
			assets->str[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_FONTS_MAX; i++)
	{
		if (assets->fon[i] != NULL)
		{
			SDL_DestroyTexture(assets->fon[i]->glyphs);
			free(assets->fon[i]);
			assets->fon[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_MESHES_MAX; i++)
	{
		if (assets->msh[i] != NULL)
		{
			if (assets->msh[i]->tris != NULL)
			{
				free(assets->msh[i]->tris);
				assets->msh[i]->tris = NULL;
			}
			free(assets->msh[i]);
			assets->msh[i] = NULL;
		}
	}

    free(assets);
	printf("\nAssets freed.");
}


void LoadSurface(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->sur[identifier] != NULL) || (identifier >= ASSETBANK_SURFACES_MAX))
	{
		printf("LoadSurface error: surface already exists at specified identifier %d, or it was out of range\n",
			   identifier);
		return;
	}
	else
	{
		SDL_Surface* surface = IMG_Load(path);
		if (surface == NULL)
		{
			printf("LoadSurface error: surface could not be loaded from %s. Error: %s\n", path, IMG_GetError());
			return;
		}
		else
		{
			assets->sur[identifier] = surface;
		}
	}
}

void LoadTexture(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path)
{
	if ((assets->tex[identifier] != NULL) || (identifier >= ASSETBANK_TEXTURES_MAX))
	{
		printf("LoadTexture error: texture at identifier %d already exists, or identifier was out of range\n",
			   identifier);
		return;
	}
	else
	{
		SDL_Texture* texture = NULL;

		SDL_Surface* temp_surface = IMG_Load(path);
		if (temp_surface == NULL)
		{
			printf("LoadTexture error: Unable to load image to surface at path %s! SDL_image Error: %s\n", path,
				   IMG_GetError());
			return;
		}
		else
		{
			// Create texture from surface pixels
			texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
			if (texture == NULL)
			{
				printf("Unable to create texture from surface at path %s! SDL Error: %s\n", path, SDL_GetError());
				return;
			}
			// Get rid of old loaded surface
			SDL_FreeSurface(temp_surface);
			assets->tex[identifier] = texture;
		}
	}
}

void LoadSound(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->sfx[identifier] != NULL) || (identifier >= ASSETBANK_SOUNDS_MAX))
	{
		printf("LoadSound error: sound already exists at specified identifier%d, or identifier was invalid.\n",
			   identifier);
		return;
	}
	else
	{
		Mix_Chunk* loaded_sound = Mix_LoadWAV(path);
		if (loaded_sound == NULL)
		{
			printf("Unable to load sound from %s! SDL Error: %s\n", path, SDL_GetError());
			return;
		}
		else
		{
			assets->sfx[identifier] = loaded_sound;
			return;
		}
	}
}

void LoadMusic(Assets* assets, i32 identifier, const char* path)
{
		if ((assets->mus[identifier] != NULL) || (identifier >= ASSETBANK_SOUNDS_MAX))
	{
		printf("LoadSound error: sound already exists at specified identifier%d, or identifier was invalid.\n",
			   identifier);
		return;
	}
	else
	{
		Mix_Music* loaded_music = Mix_LoadMUS(path);
		if (loaded_music == NULL)
		{
			printf("Unable to load sound from %s! SDL Error: %s\n", path, SDL_GetError());
			return;
		}
		else
		{
			assets->mus[identifier] = loaded_music;
			return;
		}
	}
}

void LoadCursor(Assets* assets, i32 identifier, const char* path)
{

	if ((assets->cur[identifier] != NULL) || (identifier >= ASSETBANK_CURSORS_MAX))
	{
		printf("LoadCursor error: specified cursor id %d was invalid or out of range (max cursors: %d\n", identifier, ASSETBANK_CURSORS_MAX);
	}
	else
	{
		SDL_Surface* surface = IMG_Load(path);
		
		if (surface == NULL)
		{
			printf("LoadCursor error: cursor bitmap surface could not be loaded from %s. Error: %s\n", path, IMG_GetError());
			return;
		}
		else
		{
			i2 hotspot = ZERO_I2;
			// Bytes per pixel
			const Uint8 bpp = surface->format->BytesPerPixel;
			/*
			Retrieve the address to a specific pixel
			surface->pixels  = an array containing the SDL_Surface' pixels
			surface->pitch       = the length of a row of pixels (in bytes)
			X and Y               = the offset on where on the image to retrieve the pixel; (0, 0) is the upper left corner
			*/
			for (i32 y = 0; y < ZSDL_CURSOR_BASE_SIZE; y++)
			{
				for (i32 x = 0; x < ZSDL_CURSOR_BASE_SIZE; x++)
				{
					Uint8* pPixel = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

					Uint32 PixelData = *(Uint32*)pPixel;
					Uint32* const target_pixel = (Uint32 *) ((Uint8 *) surface->pixels
                                             + y * surface->pitch
                                             + x * surface->format->BytesPerPixel);

					SDL_Color color = {0x00, 0x00, 0x00, 0xff};

					// Retrieve the RGB values of the specific pixel
					SDL_GetRGB(PixelData, surface->format, &color.r, &color.g, &color.b);
					
					if (color.r == 0xff && color.g == 0x00 && color.b == 0x00)
					{
						hotspot.x = x; hotspot.y = y;
						Uint32 transparent = SDL_MapRGBA(surface->format, 0x00, 0x00, 0x00, 0x00);
						*target_pixel = transparent;
					}
					else if (color.r == 0x00 && color.g == 0xff && color.b == 0x00)
					{
						hotspot.x = x; hotspot.y = y;
						Uint32 black = SDL_MapRGBA(surface->format, 0x00, 0x00, 0x00, 0xff);
						//*pPixel = black;
						*target_pixel = black;
					}
				}
			}
			ZSDL_Cursor* new_cursor = malloc(sizeof(ZSDL_Cursor));
			if (new_cursor != NULL)
			{
				new_cursor->source_bitmap = surface;
				new_cursor->hotspot = hotspot;
				new_cursor->cursor = SDL_CreateColorCursor(surface, hotspot.x, hotspot.y);
				assets->cur[identifier] = new_cursor;
			}
			else
			{
				printf("LoadCursor error: failed to create new cursor\n");
				return;
			}
		}
	}
}

void GenerateString(Assets* assets, i32 identifier, const char* string)
{
	if ((assets->str[identifier] != NULL) || (identifier >= ASSETBANK_STRINGS_MAX))
	{
		printf("LoadString error: string at specified identifier %d already exists, or identifier was out of range.\n",
			   identifier);
	}
	else
	{
		assets->str[identifier] = strdup(string);
	}
}

void LoadString(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->str[identifier] != NULL) || (identifier >= ASSETBANK_STRINGS_MAX))
	{
		printf("LoadString error: string at specified identifier %d already exists, or identifier was out of range.\n",
			   identifier);
	}
	else
	{
		assets->str[identifier] = "Not implemented yet.";
	}
}

void LoadFont(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path)
{
	if ((assets->fon[identifier] != NULL) || (identifier >= ASSETBANK_FONTS_MAX))
	{
		printf("LoadString error: string at specified identifier %d already exists, or identifier was out of range.\n",
			   identifier);
		return;
	}
	else
	{
		SDL_Texture* font_texture = NULL;

		SDL_Surface* temp_surface = IMG_Load(path);
		if (temp_surface == NULL)
		{
			printf("LoadTexture error: Unable to load image to surface at path %s! SDL_image Error: %s\n", path,
				   IMG_GetError());
			return;
		}
		else
		{
			// Create texture from surface pixels
			font_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
			if (font_texture == NULL)
			{
				printf("Unable to create texture from surface at path %s! SDL Error: %s\n", path, SDL_GetError());
				return;
			}
			else
			{
				zFont* new_font = malloc(sizeof(zFont));
				new_font->siz.x = temp_surface->w / 16;
				new_font->siz.y = temp_surface->h / 6;
				new_font->spacing = make_i2(0, 2); //TODO: WHAT TO DO HERE?
				new_font->glyphs = font_texture;
				assets->fon[identifier] = new_font;
			}
			// Get rid of old loaded surface
			SDL_FreeSurface(temp_surface);
			temp_surface = NULL;
			font_texture = NULL;
		}
	}
}

void MixSFX()
{
	for (i32 i = 0; i < ASSETBANK_SOUNDS_MAX; i++)
		Mix_Volume(i, MIX_MAX_VOLUME/2);
	Mix_VolumeMusic(255);

}





/*vvvvvvvvvvvvvvvvvvvvvvvvvv INPUT vvvvvvvvvvvvvvvvvvvvvvvvvv*/

Input* CreateInputManager()
{
	Input* input = malloc(sizeof(Input));
	for (i32 p = 0; p < MAX_PLAYERS; p++)
	{
		input->pcon[p] = malloc(sizeof(PlayerController));
		memset(input->pcon[p], 0, sizeof(PlayerController));
	}

	input->pcon[PLAYER_1]->actions = 0;
	input->pcon[PLAYER_1]->active = 0;
	input->pcon[PLAYER_1]->nav = ZERO_R2;
	//input->pcon[PLAYER_1]->cursor_loc = make_r2(ZSDL_INTERNAL_HALFWIDTH, ZSDL_INTERNAL_HALFHEIGHT);
	input->pcon[PLAYER_1]->cursor_loc = ZERO_R2;

	int num_joysticks = SDL_NumJoysticks();
		if ((num_joysticks >= 1) && (SDL_IsGameController(0)))
		{
			input->pcon[PLAYER_1]->keyboard = 0;
			input->pcon[PLAYER_1]->gamepad = SDL_GameControllerOpen(0);
		}
		else
		{
			input->pcon[PLAYER_1]->keyboard = 1;
		}

	SetDefaultMapping(input->pcon[PLAYER_1], input->pcon[PLAYER_1]->keyboard, 1);
		

	if (input != NULL)
		printf("\nInput manager initialized.");
	else
		printf("\nInput manager failed to initialize!");
	return input;
}

void FreeInputManager(Input* input)
{
	for (i32 p = 0; p < MAX_PLAYERS; p++)
	{
		if (input->pcon[p] != NULL)
		{
			if (input->pcon[p]->gamepad != NULL)
			{
				SDL_GameControllerClose(input->pcon[p]->gamepad);
			}
			free(input->pcon[p]);
		}	
	}
	free(input);
	printf("\nInput freed.");
}

void AddPlayer(Input* input)
{

}

void RemovePlayer(Input* input, i32 player_index)
{

}

void SetDefaultMapping(PlayerController* pcon, b8 keyboard, i32 player)
{
	if (player == 1)
	{
		if (keyboard)
		{
			pcon->mapping[ACT_PLAY] = SDL_SCANCODE_RETURN;
			pcon->mapping[ACT_1] 	= SDL_SCANCODE_SPACE;
			pcon->mapping[ACT_2] 	= SDL_SCANCODE_Z;
			pcon->mapping[ACT_3] 	= SDL_SCANCODE_X;
			pcon->mapping[ACT_4] 	= SDL_SCANCODE_C;
			pcon->mapping[ACT_NAVU] = SDL_SCANCODE_UP;
			pcon->mapping[ACT_NAVD] = SDL_SCANCODE_DOWN;
			pcon->mapping[ACT_NAVL] = SDL_SCANCODE_LEFT;
			pcon->mapping[ACT_NAVR] = SDL_SCANCODE_RIGHT;
		}
		else
		{
			pcon->mapping[ACT_PLAY] = SDL_CONTROLLER_BUTTON_START;
			pcon->mapping[ACT_1] = SDL_CONTROLLER_BUTTON_A;
			pcon->mapping[ACT_2] = SDL_CONTROLLER_BUTTON_B;
			pcon->mapping[ACT_3] = SDL_CONTROLLER_BUTTON_X;
			pcon->mapping[ACT_4] = SDL_CONTROLLER_BUTTON_Y;
			pcon->mapping[ACT_NAVU] = SDL_CONTROLLER_BUTTON_DPAD_UP;
			pcon->mapping[ACT_NAVD] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
			pcon->mapping[ACT_NAVL] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
			pcon->mapping[ACT_NAVR] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
		}
	}
}

void TickInput(Input* input)
{
	SDL_PumpEvents();
	SDL_GameControllerUpdate();
	const u8* keystate = SDL_GetKeyboardState(NULL);
	u32 mousestate	   = SDL_GetMouseState(&input->mouse_location.x, &input->mouse_location.y);
	

	for (i32 i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerController* pc = input->pcon[i];
		if (pc->active)
		{
			pc->actions <<= MAX_ACTIONS;
			if (pc->keyboard)
			{
				pc->nav.y	= keystate[pc->mapping[ACT_NAVU]] - keystate[pc->mapping[ACT_NAVD]];
				pc->nav.x	= keystate[pc->mapping[ACT_NAVR]] - keystate[pc->mapping[ACT_NAVL]];
				pc->actions |= ACT(ACT_1) 		* keystate[pc->mapping[ACT_1]];
				pc->actions |= ACT(ACT_2) 		* keystate[pc->mapping[ACT_2]];
				pc->actions |= ACT(ACT_3) 		* keystate[pc->mapping[ACT_3]];
				pc->actions |= ACT(ACT_4) 		* keystate[pc->mapping[ACT_4]];
				pc->actions |= ACT(ACT_PLAY) 	* keystate[pc->mapping[ACT_PLAY]];
				pc->actions |= ACT(ACT_NAVU) 	* keystate[pc->mapping[ACT_NAVU]];
				pc->actions |= ACT(ACT_NAVD) 	* keystate[pc->mapping[ACT_NAVD]];
				pc->actions |= ACT(ACT_NAVL) 	* keystate[pc->mapping[ACT_NAVL]];
				pc->actions |= ACT(ACT_NAVR) 	* keystate[pc->mapping[ACT_NAVR]];
			}
			else
			{
				pc->actions |= ACT(ACT_1) * SDL_GameControllerGetButton(pc->gamepad, pc->mapping[ACT_1]);
				pc->actions |= ACT(ACT_2) * SDL_GameControllerGetButton(pc->gamepad, pc->mapping[ACT_2]);
				pc->actions |= ACT(ACT_3) * SDL_GameControllerGetButton(pc->gamepad, pc->mapping[ACT_3]);
				pc->actions |= ACT(ACT_4) * SDL_GameControllerGetButton(pc->gamepad, pc->mapping[ACT_4]);
				// TODO think about whether analog and dpad should be separated or not, for now update both as one
				Sint16 axis_hori = SDL_GameControllerGetAxis(pc->gamepad, SDL_CONTROLLER_AXIS_LEFTX);
				Sint16 axis_vert = SDL_GameControllerGetAxis(pc->gamepad, SDL_CONTROLLER_AXIS_LEFTY);
				i32 dpad_u = SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
				i32 dpad_d = SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
				i32 dpad_l = SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
				i32 dpad_r = SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);


				if (dpad_u || dpad_d)
				{
					pc->nav.y = (r32)(dpad_d) - (r32)(dpad_u);
					pc->actions |= ACT(ACT_NAVU) * dpad_u;
					pc->actions |= ACT(ACT_NAVD) * dpad_d;
				}
				else
				{
					if (AbsI32(axis_vert) >= CONTROLLER_DEADZONE)
						pc->nav.y = ((r32)axis_vert / (r32)CONTROLLER_AXIS_VALUE_MAX);
					else
						pc->nav.y = 0.f;
					pc->actions |= ACT(ACT_NAVU) * (axis_vert >= (CONTROLLER_AXIS_VALUE_HALF));
					pc->actions |= ACT(ACT_NAVD) * (axis_vert <= (-CONTROLLER_AXIS_VALUE_HALF));
				}
				if (dpad_l || dpad_r)
				{
					pc->nav.x = (r32)(dpad_r) - (r32)(dpad_l);
					pc->actions |= ACT(ACT_NAVL) * (axis_hori <= (CONTROLLER_AXIS_VALUE_HALF));
					pc->actions |= ACT(ACT_NAVR) * (axis_hori >= (-CONTROLLER_AXIS_VALUE_HALF));					
				}
				else
				{
					if (AbsI32(axis_hori) >= CONTROLLER_DEADZONE)
						pc->nav.x = ((r32)axis_hori / (r32)CONTROLLER_AXIS_VALUE_MAX);
					else
						pc->nav.x = 0.f;
				}


				// u8 nav_u = (axis_vert >= (SDL_CONTROLLER_AXIS_MAX * 0.5)) 	|| SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
				// u8 nav_d = (axis_vert <= (SDL_CONTROLLER_AXIS_MAX * -0.5)) 	|| SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
				// u8 nav_l = (axis_hori <= (SDL_CONTROLLER_AXIS_MAX * -0.5)) 	|| SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
				// u8 nav_r = (axis_hori >= (SDL_CONTROLLER_AXIS_MAX * 0.5)) 	|| SDL_GameControllerGetButton(pc->gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
				// pc->actions |= ACT(ACT_NAVU) * nav_u;
				// pc->actions |= ACT(ACT_NAVD) * nav_d;
				// pc->actions |= ACT(ACT_NAVL) * nav_l;
				// pc->actions |= ACT(ACT_NAVR) * nav_r;
				// if (nav_u || nav_d)
				// {
				// 	pc->nav.y = (r32)(nav_u) + (r32)(nav_d) * -1.f;
				// }
				// else
				// {
				// 	if (AbsI32(axis_vert) >= CONTROLLER_DEADZONE)
				// 		pc->nav.y = ((r32)axis_vert / (r32)SDL_CONTROLLER_AXIS_MAX) * -1.f;
				// 	else
				// 		pc->nav.y = 0.f;
				// }
				// if (nav_l || nav_r)
				// {
				// 	pc->nav.x = (r32)(nav_r) + (r32)(nav_l) * -1.f;
				// }
				// else
				// {
				// 	if (AbsI32(axis_hori) >= CONTROLLER_DEADZONE)
				// 		pc->nav.x = (r32)axis_hori / (r32)SDL_CONTROLLER_AXIS_MAX;
				// 	else
				// 		pc->nav.x = 0.f;
				// }
			}
		}
	}
}

Controller* CreateController()
{
	printf("\nInitializing Controller...");
    Controller* controller = (Controller*)malloc(sizeof(Controller));

	controller->actions = 0;
	controller->directional_vector = ZERO_I2;
	controller->move_vector = ZERO_I2;

	if (controller != NULL)
		printf("\nController initialized.");
	else
		printf("\nController failed to initialise!");
	return controller;
}

void FreeController(Controller* controller)
{
	free(controller);
	printf("\nController freed.");
}

void CollectInput(Controller* c)
{
	c->actions <<= 32;
	SDL_PumpEvents();
	const u8* keystate = SDL_GetKeyboardState(NULL);
	
	u32 mousestate	   = SDL_GetMouseState(&c->mouse_location.x, &c->mouse_location.y);

	//@todo: rebindable keybinds
	c->move_vector.y		= -keystate[SDL_SCANCODE_W] + keystate[SDL_SCANCODE_S];
	c->move_vector.x		= -keystate[SDL_SCANCODE_A] + keystate[SDL_SCANCODE_D];
	c->directional_vector.y = -keystate[SDL_SCANCODE_UP] + keystate[SDL_SCANCODE_DOWN];
	c->directional_vector.x = -keystate[SDL_SCANCODE_LEFT] + keystate[SDL_SCANCODE_RIGHT];
	c->actions |= ACTION(A_MB_L) 	* !!(mousestate & SDL_BUTTON_LMASK);
	c->actions |= ACTION(A_MB_R) 	* !!(mousestate & SDL_BUTTON_RMASK);
	c->actions |= ACTION(A_JUMP) 	* keystate[SDL_SCANCODE_SPACE];
	c->actions |= ACTION(A_MOVL) 	* keystate[SDL_SCANCODE_LEFT];
	c->actions |= ACTION(A_MOVR) 	* keystate[SDL_SCANCODE_RIGHT];
	c->actions |= ACTION(A_MOVU) 	* keystate[SDL_SCANCODE_UP];
	c->actions |= ACTION(A_MOVD) 	* keystate[SDL_SCANCODE_DOWN];
	c->actions |= ACTION(A_PLAY) 	* keystate[SDL_SCANCODE_F9];
	c->actions |= ACTION(A_EDIT) 	* keystate[SDL_SCANCODE_F1];
	c->actions |= ACTION(A_DBUG) 	* keystate[SDL_SCANCODE_F11];
	c->actions |= ACTION(A_ESC) 	* keystate[SDL_SCANCODE_ESCAPE];
	c->actions |= ACTION(A_SHFT) 	* keystate[SDL_SCANCODE_LSHIFT];
	c->actions |= ACTION(A_ONE) 	* keystate[SDL_SCANCODE_1];
	c->actions |= ACTION(A_TWO) 	* keystate[SDL_SCANCODE_2];
	c->actions |= ACTION(A_THREE) 	* keystate[SDL_SCANCODE_3];
	c->actions |= ACTION(A_TAB) 	* keystate[SDL_SCANCODE_TAB];

}

//Returns pixelscaled mouse location clamped to render area
i2 MouseLocation(Controller* c, Viewport* viewport)
{
	u8 pixelsize				 	= GET8IN64(viewport->settings, ZSDL_SETTINGS_BYTE_PIXELSCALE);
	i2 pixel_corrected_mouse_loc 	= c->mouse_location;
	i2 camera_offset 				= make_i2(viewport->screen.x, viewport->screen.y);
	pixel_corrected_mouse_loc 		= sub_i2(pixel_corrected_mouse_loc, camera_offset);
	pixel_corrected_mouse_loc 		= i2_clamp_i2(pixel_corrected_mouse_loc, ZERO_I2, make_i2(viewport->screen.w, viewport->screen.h));
	pixel_corrected_mouse_loc.x 	/= pixelsize;
	pixel_corrected_mouse_loc.y 	/= pixelsize;
	
	return pixel_corrected_mouse_loc;
}

b8 PlayerActionPressed(PlayerController* pc, u64 action)
{
	return ((pc->actions & ACT(action)) && !(pc->actions & ACT_PRE(action)));
}

b8 PlayerActionReleased(PlayerController* pc, u64 action)
{
	return (!(pc->actions & ACT(action)) && (pc->actions & ACT_PRE(action)));
}

b8 PlayerActionHeld(PlayerController* pc, u64 action)
{
	return ((pc->actions & ACT(action)) && (pc->actions & ACT_PRE(action)));
}


b8 ActionPressed( Controller* c,  u64 action)
{
	return ((c->actions & ACTION(action)) && !(c->actions & ACTION_PRE(action)));
}

b8 ActionReleased( Controller* c,  u64 action)
{
	return (!(c->actions & ACTION(action)) && (c->actions & ACTION_PRE(action)));
}

b8 ActionHeld( Controller* c,  u64 action)
{
	return ((c->actions & ACTION(action)) && (c->actions & ACTION_PRE(action)));
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INPUT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv PARTICLES vvvvvvvvvvvvvvvvvvvvvvvvvv*/
Particles* InitParticles()
{
	Particles* particles = malloc(sizeof(Particles));
	memset(particles, 0, sizeof(Particles));
	if (particles != NULL)
		printf("\nParticles initialized.");
	else
		printf("\nParticles could not be initialized!");
	return particles;
}


b8 SpawnBubble(Particles* p, u16 lifetime, r2 pos, r2 vel, r2 acc, r32 depth, r32 initial_radius, r32 final_radius, SDL_Color initial_color, SDL_Color final_color)
{
	i32 id = -1;
	//TODO: Find way to instantly pick vacant id, removing this loop
	for (i32 i = 0; i < BUBBLES_MAX; i++)
	{
		if (!(p->bubbles[i].current_life))
		{
			id = i;
			break;
		}
	}
	if (id >= 0)
	{
		p->bubbles[id].lifetime = lifetime;
		p->bubbles[id].current_life = lifetime;
		p->bubbles[id].pos = pos;
		p->bubbles[id].vel = vel;
		p->bubbles[id].acc = acc;
		p->bubbles[id].depth = depth;
		p->bubbles[id].rad = initial_radius;
		p->bubbles[id].rad_0 = initial_radius;
		p->bubbles[id].rad_1 = final_radius;
		p->bubbles[id].r_0 = initial_color.r;
		p->bubbles[id].g_0 = initial_color.g;
		p->bubbles[id].b_0 = initial_color.b;
		p->bubbles[id].a_0 = initial_color.a;
		p->bubbles[id].r_1 = final_color.r;
		p->bubbles[id].g_1 = final_color.g;
		p->bubbles[id].b_1 = final_color.b;
		p->bubbles[id].a_1 = final_color.a;
		return 1;
	}
	else
	{
		return 0;
	}
}

b8 SpawnDot(Particles* p, u16 lifetime, r2 pos, r2 vel, r2 acc, r32 depth, SDL_Color initial_color, SDL_Color final_color)
{
	i32 id = -1;
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		if (!(p->dots[i].current_life))
		{
			id = i;
			break;
		}
	}
	if (id >= 0)
	{
		p->dots[id].lifetime = lifetime;
		p->dots[id].current_life = lifetime;
		p->dots[id].pos = pos;
		p->dots[id].vel = vel;
		p->dots[id].acc = acc;
		p->dots[id].depth = depth;
		p->dots[id].r_0 = initial_color.r;
		p->dots[id].g_0 = initial_color.g;
		p->dots[id].b_0 = initial_color.b;
		p->dots[id].a_0 = initial_color.a;
		p->dots[id].r_1 = final_color.r;
		p->dots[id].g_1 = final_color.g;
		p->dots[id].b_1 = final_color.b;
		p->dots[id].a_1 = final_color.a;
		return 1;
	}
	else
	{
		return 0;
	}
}

void TickParticles(Particles* p, u32 t, r32 dt)
{
	//TODO SPEEDUP: remove life_factor calculation (divide in inner loop), and figure out a way to remove check for each particle to see if it's alive or not (remove branching in loop)
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		if (p->dots[i].current_life != 0)
		{
			p->dots[i].current_life--;
			r32 life_factor = 1.f - (p->dots[i].current_life / (r32)p->dots[i].lifetime);
			p->dots[i].r = LerpI32(p->dots[i].r_0, p->dots[i].r_1, life_factor);
			p->dots[i].g = LerpI32(p->dots[i].g_0, p->dots[i].g_1, life_factor);
			p->dots[i].b = LerpI32(p->dots[i].b_0, p->dots[i].b_1, life_factor);
			p->dots[i].a = LerpI32(p->dots[i].a_0, p->dots[i].a_1, life_factor);
			p->dots[i].vel = add_r2(p->dots[i].vel, r2_mul_x(p->dots[i].acc, dt));
			p->dots[i].pos = add_r2(p->dots[i].pos, r2_mul_x(p->dots[i].vel, dt));
		}
	}
	for (i32 i = 0; i < BUBBLES_MAX; i++)
	{
		if (p->bubbles[i].current_life != 0)
		{
			p->bubbles[i].current_life--;
			r32 life_factor = 1.f - (p->bubbles[i].current_life / (r32)p->bubbles[i].lifetime);
			p->bubbles[i].r = LerpI32(p->bubbles[i].r_0, p->bubbles[i].r_1, life_factor);
			p->bubbles[i].g = LerpI32(p->bubbles[i].g_0, p->bubbles[i].g_1, life_factor);
			p->bubbles[i].b = LerpI32(p->bubbles[i].b_0, p->bubbles[i].b_1, life_factor);
			p->bubbles[i].a = LerpI32(p->bubbles[i].a_0, p->bubbles[i].a_1, life_factor);
			p->bubbles[i].vel = add_r2(p->bubbles[i].vel, r2_mul_x(p->bubbles[i].acc, dt));
			p->bubbles[i].pos = add_r2(p->bubbles[i].pos, r2_mul_x(p->bubbles[i].vel, dt));
			p->bubbles[i].rad = LerpR32(p->bubbles[i].rad_0, p->bubbles[i].rad_1, life_factor);
		}
	}
}



void DrawParticles(Viewport* viewport, u32 t, Particles* p)
{
	//TODO SPEEDUP: remove life checking in these loops somehow
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		if (p->dots[i].current_life != 0)
		{
			i2 cam_pos = PosToCam(p->dots[i].pos, p->dots[i].depth, viewport);
			SDL_SetRenderDrawColor(viewport->renderer, p->dots[i].r, p->dots[i].g, p->dots[i].b, p->dots[i].a);
			SDL_RenderDrawPoint(viewport->renderer, cam_pos.x, cam_pos.y);
		}
	}
	for (i32 i = 0; i < BUBBLES_MAX; i++)
	{
		if (p->bubbles[i].current_life != 0)
		{
			i2 cam_pos = PosToCam(p->bubbles[i].pos, p->bubbles[i].depth, viewport);
			SDL_SetRenderDrawColor(viewport->renderer, p->bubbles[i].r, p->bubbles[i].g, p->bubbles[i].b, p->bubbles[i].a);
			ZSDL_RenderDrawCircle(viewport, p->bubbles[i].rad * viewport->camera->zoom, cam_pos);
		}
	}
}



void FreeParticles(Particles* p)
{
	if (p != NULL)
	{
		free(p);
		p = NULL;
		printf("\nFreeParticles(): Particles freed.");
	}
	else
	{
		printf("\nFreeParticles(): Particles already NULL.");
	}
}


/*^^^^^^^^^^^^^^^^^^^^^^^^^^ DOT PARTICLES ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv CAMERA vvvvvvvvvvvvvvvvvvvvvvvvvv*/

// creates a new camera at given position and allocates memory for it, returning pointer to allocated memory
Camera* CreateCamera(r2 pos)
{
	Camera* new_camera = (Camera*)malloc(sizeof(Camera));
	if (new_camera == NULL)
	{
		return NULL;
	}
	else
	{
		new_camera->pos = pos;
		new_camera->aim = pos;
		new_camera->off = ZERO_I2;
		new_camera->zoom = 1.f;
		return new_camera;
	}
}

void FreeCamera(Camera* camera)
{
	if (camera != NULL)
	{
		free(camera);
		camera = NULL;
	}
}






i2 PosToCam( r2 pos, r32 depth, Viewport* viewport)
{
	r2 para_cpos = r2_mul_x(viewport->camera->pos, 1.f / depth);
	i2 c_cam = make_i2(ZSDL_INTERNAL_HALFWIDTH, ZSDL_INTERNAL_HALFHEIGHT);
	r2 delta = r2_mul_x(sub_r2(pos, para_cpos), viewport->camera->zoom);
	i2 result = add_i2(c_cam, r2_to_i2(delta));
	return result;

	//old, without depth, works
	// i2 c_cam = make_i2(ZSDL_INTERNAL_HALFWIDTH, ZSDL_INTERNAL_HALFHEIGHT);
	// r2 delta = r2_mul_x(sub_r2(pos, viewport->camera->pos), viewport->camera->zoom);
	// i2 result = add_i2(c_cam, r2_to_i2(delta));
	// return result;
}

r2 CamToPos( i2 cam, Viewport* viewport)
{
	i2 c_cam = make_i2(ZSDL_INTERNAL_HALFWIDTH, ZSDL_INTERNAL_HALFHEIGHT);
	r32 x = viewport->camera->pos.x + (cam.x/viewport->camera->zoom) - (c_cam.x/viewport->camera->zoom);
	r32 y = viewport->camera->pos.y + (cam.y/viewport->camera->zoom) - (c_cam.y/viewport->camera->zoom);
	return make_r2(x, y);
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ CAMERA ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv GUI vvvvvvvvvvvvvvvvvvvvvvvvvv*/
Menu CreateMenu(const char* config_section)
{
	printf("\nInitializing menu from section %s...", config_section);
    Menu menu;// = malloc(sizeof(Menu));

	//setup ini design to read data from section
	ini_t* menu_config = ini_load(GUI_DESIGN_PATH);
	i32 raw_num_btns;
	ini_sget(menu_config, config_section, "num_btn", "%d", &raw_num_btns);
	r2 margins_x_raw;
	r2 margins_y_raw;
	i2 src_raw;
	u32 slice_dim_raw;
	i32	txt_offset_y_normal_raw;
	i32	txt_offset_y_hovered_raw;
	i32	txt_offset_y_pressed_raw;
	char config_key[9] = "btn_00_l";
	i32 chr_tens = 4;
	i32 chr_ones = 5;
	i32 chr_mark = 7;

	//use data to create menu
	menu.num_buttons = raw_num_btns;
	menu.buttons = malloc(sizeof(Button) * raw_num_btns);
	for (i32 i = 0; i < raw_num_btns; i++)
	{	
		config_key[chr_ones] = (i % 10) + '0';
		config_key[chr_tens] = (i / 10) + '0';
		config_key[chr_mark] = 'l';
		//printf("accessing.. %s, %s\n", config_section, config_key);
		ini_sget(menu_config, config_section, config_key, "%f", &margins_x_raw.x);
		config_key[chr_mark] = 'r';
		ini_sget(menu_config, config_section, config_key, "%f", &margins_x_raw.y);
		config_key[chr_mark] = 't';
		ini_sget(menu_config, config_section, config_key, "%f", &margins_y_raw.x);
		config_key[chr_mark] = 'b';
		ini_sget(menu_config, config_section, config_key, "%f", &margins_y_raw.y);
		config_key[chr_mark] = 'x';
		ini_sget(menu_config, config_section, config_key, "%d", &src_raw.x);
		config_key[chr_mark] = 'y';
		ini_sget(menu_config, config_section, config_key, "%d", &src_raw.y);
		config_key[chr_mark] = 'd';
		ini_sget(menu_config, config_section, config_key, "%d", &slice_dim_raw);
		config_key[chr_mark] = 'n';
		ini_sget(menu_config, config_section, config_key, "%d", &txt_offset_y_normal_raw);
		config_key[chr_mark] = 'h';
		ini_sget(menu_config, config_section, config_key, "%d", &txt_offset_y_hovered_raw);
		config_key[chr_mark] = 'p';
		ini_sget(menu_config, config_section, config_key, "%d", &txt_offset_y_pressed_raw);
		config_key[chr_mark] = 's';
		const char* text_raw = ini_get(menu_config, config_section, config_key);

		// printf("values from ini file:\n text: %s\n margins_x: (%f, %f)\n margins_y: (%f, %f)\n src_loc: (%d, %d)\n slice dim: %d\n",
		// text_raw, margins_x_raw.x, margins_x_raw.y, margins_y_raw.x, margins_y_raw.y, src_raw.x, src_raw.y, slice_dim_raw);
		
		menu.buttons[i] = AddButton(src_raw, slice_dim_raw, margins_x_raw, margins_y_raw, text_raw, txt_offset_y_normal_raw, txt_offset_y_hovered_raw, txt_offset_y_pressed_raw);
	}

	ini_free(menu_config);

	// if (menu != NULL)
	// 	printf("menu initialized!\n");
	// else
	// 	printf("menu failed to initialize!\n");
	return menu;
}

Button AddButton(i2 src_loc, u32 slice_dim, r2 margins_x, r2 margins_y, const char* text, i8 txt_offset_y_normal, i8 txt_offset_y_hovered, i8 txt_offset_y_pressed)
{

	//raw data represents data from design config file, data is loaded in the create menu function and passed as
	// arguments here
	//margin_x.x is left edge of button and .y is right edge of button in percent of screen real estate, y.x is top and y.y is bottom

	i2 rect_loc = {margins_x.x * ZSDL_INTERNAL_WIDTH, margins_y.x * ZSDL_INTERNAL_HEIGHT};
	//i2 rect_dim = {ZSDL_INTERNAL_WIDTH - (margins_x.y * ZSDL_INTERNAL_WIDTH) - rect_loc.x, ZSDL_INTERNAL_HEIGHT - (margins_y.y * ZSDL_INTERNAL_HEIGHT) - rect_loc.y};
	i2 rect_dim = {(margins_x.y - margins_x.x) * ZSDL_INTERNAL_WIDTH, (margins_y.y  - margins_y.x) * ZSDL_INTERNAL_HEIGHT};

	//SDL_Rect dst = {rect_loc.x, rect_loc.y, rect_dim.x, rect_dim.y};

	Button btn;
	btn.state = 0;
	btn.src_loc = src_loc;
	btn.dst_loc = rect_loc;
	btn.dst_siz = rect_dim;
	btn.slice_dim = slice_dim;
	btn.txt = strdup(text);
	btn.txt_offset_y_current = txt_offset_y_normal;
	btn.txt_offset_y_normal = txt_offset_y_normal;
	btn.txt_offset_y_hovered = txt_offset_y_hovered;
	btn.txt_offset_y_pressed = txt_offset_y_pressed;

	// printf("\nprocessed values from button:\n src: (%d, %d)\n dst_loc: (%d, %d)\n dst_siz: (%d, %d)\n slice_dim: %d\n txt: %s\n\n",
	// btn.src_loc.x, 
	// btn.src_loc.y, 
	// btn.dst_loc.x,
	// btn.dst_loc.y,
	// btn.dst_siz.x,
	// btn.dst_siz.y,
	// btn.slice_dim,
	// btn.txt
	// );
	return btn;
}









i32 TickMenu(Menu menu, i2 mouse_location, Controller* controller, Input* input)
{
	SDL_Point mpoint = {mouse_location.x, mouse_location.y};
	SDL_Point p1_pt = {input->pcon[PLAYER_1]->cursor_loc.x, input->pcon[PLAYER_1]->cursor_loc.y};
	//SDL_Point p2_pt = {mouse_location.x, mouse_location.y};
	i32 button_was_pressed = -1;
	static b8 transition_allowed[BUTTON_STATE_MAX*BUTTON_STATE_MAX] = 
	{ //FROM:   inact	active	hovered	pressed	held	release	||TO:
				1,      1,      1,      1,      1,      1,    	//inactive
				1,      1,      1,      0,      1,      1,    	//active
				0,      1,      1,      0,      0,      1,    	//hovered
				0,      0,      1,      1,      0,      0,    	//pressed
				0,      0,      0,      1,      1,      0,    	//held
				0,      0,      0,      1,      1,      0,    	//released
	};
	for (i32 i = 0; i < menu.num_buttons; i++)
	{
		//u8 btn_state_old = GET4IN8(menu.buttons[i].state, BUTTON_STATE_POS_OLD);
		u8 btn_state_now = GET4IN8(menu.buttons[i].state, BUTTON_STATE_POS_NOW);
		u8 btn_state_new = btn_state_now;
		//printf("button num %d's state is %d", i, btn_state_now);
		SDL_Rect hitbox = {menu.buttons[i].dst_loc.x, menu.buttons[i].dst_loc.y, menu.buttons[i].dst_siz.x, menu.buttons[i].dst_siz.y};

		if (btn_state_now) // inactive if 0
		{
			if (SDL_PointInRect(&mpoint, &hitbox) || SDL_PointInRect(&p1_pt, &hitbox))
			{
				if (ActionPressed(controller, A_MB_L) || (PlayerActionPressed(input->pcon[PLAYER_1], ACT_1)))
				{
					btn_state_new = BUTTON_STATE_PRESSED;
				}
				else if (ActionHeld(controller, A_MB_L) || (PlayerActionHeld(input->pcon[PLAYER_1], ACT_1)))
				{
					btn_state_new = BUTTON_STATE_HELD;
				}
				else if (ActionReleased(controller, A_MB_L) || (PlayerActionReleased(input->pcon[PLAYER_1], ACT_1)))
				{
					btn_state_new = BUTTON_STATE_RELEASED;
				}
				else
				{
					btn_state_new = BUTTON_STATE_HOVERED;
				}

			}
			else
			{
				btn_state_new = BUTTON_STATE_ACTIVE;
			}

			if (btn_state_now == btn_state_new)
			{
				//printf("Next state was same as previous.\n");
				menu.buttons[i].state = MAKE8FROM4(btn_state_now, btn_state_new);
			}
			else
			{
				if (transition_allowed[btn_state_now + btn_state_new * BUTTON_STATE_MAX])
				{
					switch (btn_state_now) //exit current state, cleanup
					{
						case BUTTON_STATE_INACTIVE:
							break;
						case BUTTON_STATE_ACTIVE:
							break;
						case BUTTON_STATE_HOVERED:
							break;
						case BUTTON_STATE_PRESSED:
							break;
						case BUTTON_STATE_HELD:
							break;
						case BUTTON_STATE_RELEASED:
							break;
					}
					switch (btn_state_new) //enter next state, setup
					{
						case BUTTON_STATE_INACTIVE:
							break;
						case BUTTON_STATE_ACTIVE:
							menu.buttons[i].src_loc.x = 0;
							menu.buttons[i].txt_offset_y_current = menu.buttons[i].txt_offset_y_normal;
							break;
						case BUTTON_STATE_HOVERED:
							menu.buttons[i].src_loc.x = menu.buttons[i].slice_dim * 3;
							menu.buttons[i].txt_offset_y_current = menu.buttons[i].txt_offset_y_hovered;
							break;
						case BUTTON_STATE_PRESSED:
							menu.buttons[i].src_loc.x = menu.buttons[i].slice_dim * 6;
							menu.buttons[i].txt_offset_y_current = menu.buttons[i].txt_offset_y_pressed;
							button_was_pressed = i;
							break;
						case BUTTON_STATE_HELD:
							menu.buttons[i].src_loc.x = menu.buttons[i].slice_dim * 6;
							break;
						case BUTTON_STATE_RELEASED:
							menu.buttons[i].src_loc.x = 0;
							break;
					}
					//printf("BTN from %s to %s\n", ButtonStateName(btn_state_now), ButtonStateName(btn_state_new));
					menu.buttons[i].state = MAKE8FROM4(btn_state_now, btn_state_new);
				}
				else
				{
					//printf("Button state change from %s \tto %s was deemed illegal!\n", ButtonStateName(btn_state_now), ButtonStateName(next_state));
					menu.buttons[i].state = MAKE8FROM4(btn_state_now, btn_state_now);
				}
			}
		}
	}
	return button_was_pressed;
}

void DrawMenu(Menu menu, Viewport* viewport, Assets* assets)
{
	for (i32 i = 0; i < menu.num_buttons; i++)
	{
		u8 btn_state_now = GET4IN8(menu.buttons[i].state, BUTTON_STATE_POS_NOW);

		if (btn_state_now) // inactive if 0
		{
			//draw nineslice
			DrawNineSliced(viewport, assets->tex[T_UI_ATLAS], menu.buttons[i].src_loc, menu.buttons[i].dst_loc, menu.buttons[i].dst_siz, menu.buttons[i].slice_dim);
			// i2 txt_siz = {strlen(menu.buttons[i].txt)*assets->fon[FONT_ID_ZSYS]->siz.x, assets->fon[FONT_ID_ZSYS]->siz.y};
			SDL_Rect txt_dst = {menu.buttons[i].dst_loc.x, menu.buttons[i].dst_loc.y + menu.buttons[i].txt_offset_y_current, menu.buttons[i].dst_siz.x, menu.buttons[i].dst_siz.y};
			// {
			// 	menu.buttons[i].dst_loc.x + menu.buttons[i].dst_siz.x * 0.5f - txt_siz.x * 0.5f,
			// 	menu.buttons[i].dst_loc.y + menu.buttons[i].dst_siz.y * 0.5f - txt_siz.y * 0.5f,
			// };
			DrawTextScreenCentered(viewport, assets->fon[FONT_ID_ZSYS], COLOR_BLACK, txt_dst, menu.buttons[i].txt);

		}
	}
}

void ToggleMenu(Menu* menu, b8 enable)
{
	for (i32 j = 0; j < menu->num_buttons; j++)
		menu->buttons[j].state = enable;
}

void FreeMenus(Menu* menus)
{
	if (menus != NULL)
	{
		for (i32 j = 0; j < MAX_MENUS; j++)
		{
			for (i32 i = 0; i < menus[j].num_buttons; i++)
			{
				if (menus[j].buttons[i].txt != NULL)
					free(menus[j].buttons[i].txt);
			}
		}
		free(menus);
		menus = NULL;
	}
	printf("\nMenus freed.");
}



char* ButtonStateName(E_BUTTON_STATE state)
{
    switch (state)
	{
		case BUTTON_STATE_INACTIVE :
			return "[Button Inactive]";
    	case BUTTON_STATE_ACTIVE   :
			return "[Button Active]";
    	case BUTTON_STATE_HOVERED  :
			return "[Button Hovered]";
    	case BUTTON_STATE_PRESSED  :
			return "[Button Pressed]";
    	case BUTTON_STATE_HELD     :
			return "[Button Held]";
    	case BUTTON_STATE_RELEASED :
			return "[Button Released]";
		default:
			return "[unknown button state!]";
	}
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ GUI ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv RENDER SUPPORT FUNCTIONS vvvvvvvvvvvvvvvvvvvvvvvvvv*/

void SetCursor(Viewport* viewport, Assets* assets, u8 new_cursor)
{
	if (assets->cur[new_cursor] != NULL)
	{
		SET8IN64(new_cursor, &viewport->settings, ZSDL_SETTINGS_BYTE_ACTIVE_CURSOR);
		SDL_SetCursor(assets->cur[new_cursor]->cursor);
	}
}

void RefreshCursors(Viewport* viewport, Assets* assets)
{
	i32 pixelscale = GET8IN64(viewport->settings, ZSDL_SETTINGS_BYTE_PIXELSCALE);
	for (i32 i = 0; i < ASSETBANK_CURSORS_MAX; i++)
	{
		if (assets->cur[i] != NULL)
		{
			SDL_Surface* cursor_scaled = SDL_CreateRGBSurface(0, ZSDL_CURSOR_BASE_SIZE * pixelscale, ZSDL_CURSOR_BASE_SIZE * pixelscale, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
			SDL_BlitScaled(assets->cur[i]->source_bitmap, NULL, cursor_scaled, NULL);
			SDL_FreeCursor(assets->cur[i]->cursor);
			assets->cur[i]->cursor = SDL_CreateColorCursor(cursor_scaled, assets->cur[i]->hotspot.x * pixelscale, assets->cur[i]->hotspot.y * pixelscale);
			SDL_FreeSurface(cursor_scaled);
		}
	}
	SetCursor(viewport, assets, GET8IN64(viewport->settings, ZSDL_SETTINGS_BYTE_ACTIVE_CURSOR));

}

void ToggleFullscreen(Viewport* viewport)
{
	int current_display_index = SDL_GetWindowDisplayIndex(viewport->window);
	SDL_DisplayMode current_display_mode;
	SDL_GetCurrentDisplayMode(current_display_index, &current_display_mode);
	if (SDL_GetWindowFlags(viewport->window) & SDL_WINDOW_FULLSCREEN)
	{
		SDL_SetWindowFullscreen(viewport->window, 0);
		SDL_SetWindowPosition(viewport->window, SDL_WINDOWPOS_CENTERED_DISPLAY(current_display_index), SDL_WINDOWPOS_CENTERED_DISPLAY(current_display_index));
	}
	else
		SDL_SetWindowFullscreen(viewport->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		//TODO: Implement true fullscreen, requires calls to GetCurrentDisplayMode, SetCurrentDisplayMode.. handle multiple monitors


	
}


void CleanRenderTargets(Viewport* viewport)
{
	SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetRenderTarget(viewport->renderer, NULL);
	SDL_RenderClear(viewport->renderer);
	//clear all layers except ui and fade (last two renderlayers), clear those manutally elsewhere
	// we do it in reverse so we end up with renderlayer_background to begin next frame with

	for  (i32 i = ZSDL_RENDERLAYERS_MAX - 1; i >= 0; i--) 
	{
		SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[i]);
		SDL_RenderClear(viewport->renderer);
	}
}

void FinalizeRenderAndPresent(Viewport* viewport)
{
	SDL_SetRenderTarget(viewport->renderer, NULL);
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_BLEND);
	for (i32 i = 0; i < ZSDL_RENDERLAYERS_MAX; i++)
	{
		//if (i == ZSDL_RENDERLAYER_POST_PROCESS_A || i == ZSDL_RENDERLAYER_POST_PROCESS_B)
		//	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_NONE);
		SDL_RenderCopy(viewport->renderer, viewport->render_layer[i], NULL, &viewport->screen);
	}
    SDL_RenderPresent(viewport->renderer);
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_NONE);
}

void ComputePixelScale(Viewport* viewport)
{
	i32 screen_width, screen_height;
	if (SDL_GetWindowFlags(viewport->window) & SDL_WINDOW_FULLSCREEN)
	{
		SDL_DisplayMode current;
		int display_index = SDL_GetWindowDisplayIndex(viewport->window);
		int success		  = SDL_GetCurrentDisplayMode(display_index, &current);

		if (success != 0)
		{
			// In case of error...
			SDL_Log("Could not get display mode for video display #%d: %s", display_index, SDL_GetError());
		}
		else
		{
			// On success, print the current display mode.
			//SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", display_index, current.w, current.h, current.refresh_rate);
			screen_height = current.h;
			screen_width  = current.w;
		}
	}
	else
	{
		SDL_GetWindowSize(viewport->window, &screen_width, &screen_height);
	}
	i32 renderscale_x = screen_width / ZSDL_INTERNAL_WIDTH;
	i32 renderscale_y = screen_height / ZSDL_INTERNAL_HEIGHT;
	u8 renderscale = MinI32(renderscale_x, renderscale_y);
	//u8 renderscale = (screen_width < screen_height) * (screen_width / ZSDL_INTERNAL_WIDTH) +
	//				 (screen_width >= screen_height) * (screen_height / ZSDL_INTERNAL_HEIGHT);
	if (renderscale == 0)
		renderscale = 1;
	SET8IN64(renderscale, &viewport->settings, ZSDL_SETTINGS_BYTE_PIXELSCALE);
}

void CalculateScreen(Viewport* viewport)
{
	i32 screen_width, screen_height;
	if (SDL_GetWindowFlags(viewport->window) & SDL_WINDOW_FULLSCREEN)
	{
		SDL_DisplayMode current;
		int display_index = SDL_GetWindowDisplayIndex(viewport->window);
		int success		  = SDL_GetCurrentDisplayMode(display_index, &current);

		if (success != 0)
		{
			// In case of error...
			SDL_Log("Could not get display mode for video display #%d: %s", display_index, SDL_GetError());
		}
		else
		{
			// On success, print the current display mode.
			SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", display_index, current.w, current.h,
					current.refresh_rate);
			screen_height = current.h;
			screen_width  = current.w;
		}
	}
	else
	{
		SDL_GetWindowSize(viewport->window, &screen_width, &screen_height);
	}
	i32 pixelscale = GET8IN64(viewport->settings, ZSDL_SETTINGS_BYTE_PIXELSCALE);
	viewport->screen.w = pixelscale * ZSDL_INTERNAL_WIDTH;
	viewport->screen.h = pixelscale * ZSDL_INTERNAL_HEIGHT;
	viewport->screen.x = (screen_width / 2) - (viewport->screen.w / 2);
	viewport->screen.y = (screen_height / 2) - (viewport->screen.h / 2);
}

void DrawNumber(Viewport* viewport, SDL_Texture* texture, u32 number, i2 size_src, i2 size_dst, i2 location, u32 max_digits)
{
	SDL_Rect src = {0, 0, size_src.x, size_src.y};
	SDL_Rect dst = {location.x, location.y, size_dst.x, size_dst.y};
	u32 digits[max_digits];
	memset(digits, 0, max_digits*sizeof(u32));
	i32 k = 0;
	i32 r = 0;
	i32 c = 0;
	while(number!=0)
	{
		r = number%10;
		digits[k] = r;
		k++;
		number = number/10;
		c++;
	}
	for (i32 i = 0; i < max_digits; i++)
	{
		src.y = size_src.y * digits[i];	
		dst.x = location.x + (max_digits * size_dst.x) - (size_dst.x * i);
		SDL_RenderCopy(viewport->renderer, texture, &src, &dst);
	}
}

void ZSDL_RenderDrawCircle(Viewport* viewport, u32 radius, i2 center)
{
	u32 diameter = (radius * 2);

	i32 x = (radius - 1);
	i32 y = 0;
	i32 tx = 1;
	i32 ty = 1;
	i32 error = (tx - diameter);


	while (x >= y)
	{

      	//  Each of the following renders an octant of the circle
      	SDL_RenderDrawPoint(viewport->renderer, center.x + x, center.y - y);
      	SDL_RenderDrawPoint(viewport->renderer, center.x + x, center.y + y);
      	SDL_RenderDrawPoint(viewport->renderer, center.x - x, center.y - y);
      	SDL_RenderDrawPoint(viewport->renderer, center.x - x, center.y + y);
      	SDL_RenderDrawPoint(viewport->renderer, center.x + y, center.y - x);
      	SDL_RenderDrawPoint(viewport->renderer, center.x + y, center.y + x);
      	SDL_RenderDrawPoint(viewport->renderer, center.x - y, center.y - x);
      	SDL_RenderDrawPoint(viewport->renderer, center.x - y, center.y + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

void DrawNineSliced(Viewport* viewport, struct SDL_Texture* source_texture, i2 src_loc, i2 dst_loc, i2 dst_siz, i32 slice_dimensions)
{

	SDL_Rect src_slice = { src_loc.x, src_loc.y, slice_dimensions, slice_dimensions };
	SDL_Rect dst_slice = { dst_loc.x, dst_loc.y, slice_dimensions, slice_dimensions };

	i32 x_coord[3] = { dst_loc.x, dst_loc.x + slice_dimensions, dst_loc.x + dst_siz.x - slice_dimensions };
	i32 y_coord[3] = { dst_loc.y, dst_loc.y + slice_dimensions, dst_loc.y + dst_siz.y - slice_dimensions };
	i32 widths[3] = { slice_dimensions, dst_siz.x - (2 * slice_dimensions), slice_dimensions };
	i32 heights[3] = { slice_dimensions, dst_siz.y - (2 * slice_dimensions), slice_dimensions };
	
	for (i32 v = 0; v < 3; v++)
	{
		for (i32 u = 0; u < 3; u++)
		{
			src_slice.x = src_loc.x + u * slice_dimensions;
			src_slice.y = src_loc.y + v * slice_dimensions;

			dst_slice.x = x_coord[u];
			dst_slice.y = y_coord[v];
			dst_slice.w = widths[u];
			dst_slice.h = heights[v];

			SDL_RenderCopy(viewport->renderer, source_texture, &src_slice, &dst_slice);
		}
	}
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ RENDER SUPPORT FUNCTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv FONT TEXT DRAWING vvvvvvvvvvvvvvvvvvvvvvvvvv*/
void DrawTextWorld(Viewport* viewport, zFont* font, SDL_Color color, r2 pos, r32 depth, const char* text)
{
	i32 i = 0;
	i2 screen_pos = PosToCam(pos, depth, viewport);
	SDL_Rect src = {0, 0, font->siz.x, font->siz.y};
	SDL_Rect dst = {screen_pos.x, screen_pos.y, font->siz.x, font->siz.y};
	SDL_SetTextureColorMod(font->glyphs, color.r, color.g, color.b);
	while(text[i] != '\0')
    {
        i32 glyph_idx = text[i] - ZFONT_ASCII_OFFSET;
		src.x = (glyph_idx % ZFONT_DEFAULT_MAX_COL) * font->siz.x;
		src.y = (glyph_idx / ZFONT_DEFAULT_MAX_COL) * font->siz.y;
		dst.x = screen_pos.x + i * font->siz.x + font->spacing.x;
		dst.y = screen_pos.y;//+ i * font->siz.y + font->spacing.y;

        SDL_RenderCopy(viewport->renderer, font->glyphs, &src, &dst);
        i++;
    }
}

void DrawTextScreenCentered(Viewport* viewport, zFont* font, SDL_Color color, SDL_Rect dst, const char* text)
{
	i32 i = 0;
	i32 len = strlen(text);
	i32 w = len * font->siz.x;
	i32 h = font->siz.y;
	i2 loc = make_i2(dst.x + ((dst.w/2) - (w/2)), dst.y + ((dst.h/2) - (h/2)));
	i2 screen_pos = loc;
	SDL_Rect src = {0, 0, font->siz.x, font->siz.y};
	SDL_Rect _dst = {loc.x, loc.y, font->siz.x, font->siz.y};
	SDL_SetTextureColorMod(font->glyphs, color.r, color.g, color.b);
	while(text[i] != '\0')
    {
        i32 glyph_idx = text[i] - ZFONT_ASCII_OFFSET;
		src.x = (glyph_idx % ZFONT_DEFAULT_MAX_COL) * font->siz.x;
		src.y = (glyph_idx / ZFONT_DEFAULT_MAX_COL) * font->siz.y;
		_dst.x = screen_pos.x + i * font->siz.x + font->spacing.x;
		_dst.y = screen_pos.y;//+ i * font->siz.y + font->spacing.y;

        SDL_RenderCopy(viewport->renderer, font->glyphs, &src, &_dst);
        i++;
    }
}

void DrawTextScreen(Viewport* viewport, zFont* font, SDL_Color color, i2 loc, const char* text)
{
	i32 i = 0;
	i32 l = 0;
	i32 c = 0;
	i2 screen_pos = loc;
	SDL_Rect src = {0, 0, font->siz.x, font->siz.y};
	SDL_Rect _dst = {loc.x, loc.y, font->siz.x, font->siz.y};
	SDL_SetTextureColorMod(font->glyphs, color.r, color.g, color.b);
	while(text[i] != '\0')
    {
		if (text[i] == '}')
		{
			l++;
			c = 0;
		}
        else 
		{
			i32 glyph_idx = text[i] - ZFONT_ASCII_OFFSET;
			src.x = (glyph_idx % ZFONT_DEFAULT_MAX_COL) * font->siz.x;
			src.y = (glyph_idx / ZFONT_DEFAULT_MAX_COL) * font->siz.y;
			_dst.x = screen_pos.x + c * font->siz.x + font->spacing.x;
			_dst.y = screen_pos.y + (l * (font->siz.y + font->spacing.y));

        	SDL_RenderCopy(viewport->renderer, font->glyphs, &src, &_dst);
			c++;
		}
        i++;
    }
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ FONT TEXT DRAWING ^^^^^^^^^^^^^^^^^^^^^^^^^^*/



/*vvvvvvvvvvvvvvvvvvvvvvvvvv ANALYSIS FUNCTIONS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
void DPrintMouseLoc(Controller* controller, Viewport* viewport, Assets* assets)
{
	char txt_mraw[50];
    char txt_mpos[50];
    char txt_mcam[50];
    i2 mraw = MouseLocation(controller, viewport);
    r2 mpos = CamToPos(mraw, viewport);
    i2 mcam = PosToCam(mpos, 1.f, viewport);
    sprintf(txt_mraw, "mraw: (%d, %d)", mraw.x, mraw.y);
    sprintf(txt_mpos, "mpos: (%f, %f)", mpos.x, mpos.y);
    sprintf(txt_mcam, "mcam: (%d, %d)", mcam.x, mcam.y);
    DrawTextScreenCentered(viewport, assets->fon[0], COLOR_WHITE, (SDL_Rect){3, 3, 1, 1}, txt_mraw);
    DrawTextScreenCentered(viewport, assets->fon[0], COLOR_WHITE, (SDL_Rect){3, 3 + assets->fon[0]->siz.y, 1, 1}, txt_mpos);
    DrawTextScreenCentered(viewport, assets->fon[0], COLOR_WHITE, (SDL_Rect){3, 3+ assets->fon[0]->siz.y*2, 1, 1}, txt_mcam);
}
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ ANAALYSIS FUNCTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/