#include <stdio.h>
#include "assets.h"
#include "game.h"
#include "update.h"
#include "render.h"
#include "zsdl.h"

#define PRINT_DBG_GAMESTATE 0


int main(int argc, char* argv[])
{
/*vvvvvvvvvvvvvvvvvvvvvvvvvv INIT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	u32 t			 = 0;	  // tick store
	const u32 dt_ms	 = 10;	  // tick duration ms
	const r32 dt_sec = 0.01f; // tick duration seconds

	//Gamestate gamestate_old = GAMESTATE_INIT;
	Gamestate gamestate_now = GAMESTATE_INIT;
	Gamestate gamestate_new = GAMESTATE_INIT;

	SetupSDL();
	Viewport* viewport = CreateViewport("zengine");
	Game* game = CreateGame();
	Controller* controller = CreateController();
	Assets* assets = CreateAssets(viewport);
	//Menu* menu = CreateMenu();
	viewport->camera = CreateCamera(ZERO_R2);
	Dots* dots = initDots();
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INIT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv LOAD ASSETS vvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ LOAD ASSETS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	// SDL clock is accurate to 10ms, i.e. it counts in 10 ms. maybe switch to more accurate one 
	u32 current_time	 = SDL_GetTicks(); 
	u32 time_accumulator = 0;

///*vvvvvvvvvvvvvvvvvvvvvvvvvv GAMELOOP BEGIN vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	while (gamestate_now != GAMESTATE_EXIT)
	{
		u32 new_time   = SDL_GetTicks();
		u32 frame_time = new_time - current_time;
		if (frame_time > 250)
			frame_time = 250; // avoid spiral of death scenario
		current_time = new_time;
		time_accumulator += frame_time;

/* LOGIC UPDATE IN FIXED TIMESTEPS */
		while (time_accumulator >= dt_ms)
		{
			//gamestate_old = gamestate_now;
			CollectInput(controller);
			
/* TRANSITION GAMESTATE BEGIN */
		    if (gamestate_now != gamestate_new)
    		{
   				static b8 transition_allowed[NUMBER_OF_GAMESTATES*NUMBER_OF_GAMESTATES] = 
    			{ //FROM	init    main    play    lose	vict	edit	exit	  TO
                			1,      0,      0,      0,      0,		0,		0,		//init
                			1,      1,      1,      1,      1,      1,		0,		//main
                			0,      1,      1,      1,      1,      1,		0,		//play
                			0,      0,      1,      1,      0,      0,		0,		//lose
                			0,      0,      1,      0,      1,      0,		0,		//vict
                			0,      1,      1,      0,      0,      1,		0,		//edit
                			0,      1,      1,      1,      1,      1,		1		//exit
    			};

    			if (transition_allowed[gamestate_now + gamestate_new * NUMBER_OF_GAMESTATES])
	    		{

/*	exit and cleanup current state	*/
#if PRINT_DBG_GAMESTATE
printf("Game exiting state \t%s...\n", GamestateName(gamestate_now));
#endif
	        		switch (gamestate_now) 
	        		{
	            		case GAMESTATE_INIT:
	            		    break;
	            		case GAMESTATE_MAIN:
	            		    break;
	            		case GAMESTATE_PLAY:
	            		    break;
	            		case GAMESTATE_LOSE:
	            		    break;
	            		case GAMESTATE_VICT:
	            		    break;
	            		case GAMESTATE_EDIT:
	            		    break;
	            		case GAMESTATE_EXIT:
	            		    break;
	        		}
					//gamestate_old = gamestate_now;

/*	enter and setup next state	*/
#if PRINT_DBG_GAMESTATE
printf("Game entering state \t%s...\n", GamestateName(gamestate_new));
#endif
	        		switch (gamestate_new) 
	        		{
	            		case GAMESTATE_INIT:
	            		    break;
	            		case GAMESTATE_MAIN:
	            		    break;
	            		case GAMESTATE_PLAY:
	            		    break;
	            		case GAMESTATE_LOSE:
	            		    break;
	            		case GAMESTATE_VICT:
	            		    break;
	            		case GAMESTATE_EDIT:
	            		    break;
	            		case GAMESTATE_EXIT:
	            		    break;
	        		}
#if PRINT_DBG_GAMESTATE
printf("Gamestate change complete.\n");
#endif
					gamestate_now = gamestate_new;
	    		} // end if transition allowed
	    		else //keep current state, but push back and update old state
	    		{
					//gamestate_old = gamestate_now;
#if PRINT_DBG_GAMESTATE
printf("Gamestate change from %s \tto %s was deemed illegal!\n", GamestateName(gamestate_now), GamestateName(gamestate_new));
#endif
	    		}
			}
/* TRANSITION GAMESTATE END */

/* PERFORM STATE LOGIC UPDATE */
			switch (gamestate_now)
			{
	            case GAMESTATE_INIT:
					gamestate_new = GAMESTATE_MAIN;
	                break;
	            case GAMESTATE_MAIN:
					gamestate_new = UpdateMain(t, dt_sec, viewport, game, controller, dots, assets);
	                break;
	            case GAMESTATE_PLAY:
					gamestate_new = UpdatePlay(t, dt_sec, viewport, game, controller, dots, assets);
	                break;
	            case GAMESTATE_LOSE:
					gamestate_new = UpdateLose(t, dt_sec, viewport, game, controller, dots, assets);
	                break;
	            case GAMESTATE_VICT:
#if PRINT_DBG_GAMESTATE
printf("Gamestate entered state it shouldn't be in: %s \tto %s !\n", GamestateName(gamestate_old), GamestateName(gamestate_now));
#endif					
					gamestate_new = GAMESTATE_EXIT;
	                break;
	            case GAMESTATE_EDIT:
#if PRINT_DBG_GAMESTATE
printf("Gamestate entered state it shouldn't be in: %s \tto %s !\n", GamestateName(gamestate_old), GamestateName(gamestate_now));
#endif					
					gamestate_new = GAMESTATE_EXIT;
	                break;
	            case GAMESTATE_EXIT:
		            break;
			}
			tickDots(dots, t, dt_sec);
			// advance time
			t++;
			time_accumulator -= dt_ms;
		} //logic update end
		
/* RENDER UPDATE */
		//@TODO: calculate interpolation value from last render to smooth rendering
		//prev_state * (1-t) + curr_state * t
		CleanRenderTargets(viewport);
		switch (gamestate_now)
		{
			case GAMESTATE_INIT:
			break;
			case GAMESTATE_MAIN:
				RenderMain(viewport, game, controller, dots, assets);
			break;
			case GAMESTATE_PLAY:
				RenderPlay(viewport, game, controller, dots, assets);
			break;
			case GAMESTATE_LOSE:
				RenderLose(viewport, game, controller, dots, assets);
			break;
			case GAMESTATE_VICT:
			break;
			case GAMESTATE_EDIT:
			break;
			case GAMESTATE_EXIT:
			break;
		}
		SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
		DrawDots(viewport, t, dots);
		FinalizeRenderAndPresent(viewport);
	} //main loop end
#if PRINT_DBG_GAMESTATE
printf("\n~~~Exiting game!~~~\n");
#endif		
	// free all things
	FreeDots(dots);
//	FreeMenu(menu);
	FreeController(controller);
	FreeAssets(assets);
	FreeViewport(viewport);
	FreeGame(game);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}