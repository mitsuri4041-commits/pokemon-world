#include <stdio.h>
#include <string.h>

#include "game.h"
#include "world.h"
#include "player.h"
#include "npc.h"
#include "quest.h"
#include "save.h"

void game_init(Game *game)
{
    if (game == NULL)
        return;

    memset(game, 0, sizeof(Game));

    game->running = true;

    player_init(&game->player);
    world_init(game);
    npc_init();
    quest_init(game);
    quest_start(game, 1);
    save_init();

    snprintf(
        game->notice,
        sizeof(game->notice),
        "Welcome to Pokemon World!"
    );

    load_game(game);
}

void game_update(Game *game)
{
    if (game == NULL || !game->running)
        return;

    world_update(game);
    npc_update(game);
    quest_update(game);

    u32 keys = hidKeysDown();

    if (keys & KEY_START)
    {
        if (save_game(game))
        {
            snprintf(
                game->notice,
                sizeof(game->notice),
                "Game saved successfully!"
            );
        }
        else
        {
            snprintf(
                game->notice,
                sizeof(game->notice),
                "Save failed!"
            );
        }
    }

    if (keys & KEY_SELECT)
    {
        if (!load_game(game))
        {
            snprintf(
                game->notice,
                sizeof(game->notice),
                "No save data found!"
            );
        }
    }

    if (keys & KEY_B)
    {
        game->running = false;
    }
}

void game_render(Game *game)
{
    if (game == NULL)
        return;

    world_render(game);

    printf("\n\n");
    printf("================================\n");
    printf("NOTICE:\n%s\n", game->notice);
    printf("================================\n");

    printf("\nSTART: Save");
    printf("\nSELECT: Load");
    printf("\nB: Exit");
    printf("\n");
}

void game_shutdown(Game *game)
{
    if (game == NULL)
        return;

    save_game(game);
}
