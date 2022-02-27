#ifndef ZENGINE_H
#define ZENGINE_H


#include "assets.h"
#include "game.h"
#include "zgrid.h"
#include "zmath.h"
#include "zsdl.h"


typedef struct
{
	Viewport* viewport;
	Game* game;
	Controller* controller;
	Input* input;
	Assets* assets;
	Particles* particles;
	Menu* menus;
	Gamestate gamestate_old;
	Gamestate gamestate_now;
	Gamestate gamestate_new;
	u32 t_0_gamestate_change;
	i32  (*update[NUMBER_OF_GAMESTATES])(u32, r32, void*);
	void (*render[NUMBER_OF_GAMESTATES])(u32, r32, void*);
} zEngine;


#endif