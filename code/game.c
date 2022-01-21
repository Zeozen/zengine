#include <stdio.h>
#include "game.h"

char* GetGamestateName(Gamestate state)
{
	switch (state)
	{
		case GAMESTATE_INIT:
			return "[Gamestate Init]";
		case GAMESTATE_MAIN:
			return "[Gamestate Main]";
		case GAMESTATE_PLAY:
			return "[Gamestate Play]";
		case GAMESTATE_LOSE:
			return "[Gamestate Lose]";
		case GAMESTATE_VICT:
			return "[Gamestate Victory]";
		case GAMESTATE_EDIT:
			return "[Gamestate Edit]";
		case GAMESTATE_EXIT:
			return "[Gamestate Exit]";
		default:
			return "[Gamestate Unknown]";
	}
}


Game* CreateGame()
{
    Game* new_game = malloc(sizeof(Game));
	RestartGame(new_game);

    return new_game;
}

void FreeGame(Game* game)
{
    if (game != NULL)
        free(game);
    printf("Game freed.\n");
}

void RestartGame(Game* game)
{
	for (i32 i = 0; i < GAMEDATA_MAX; i++)
	{
		game->data[i] = 0;
	}
}