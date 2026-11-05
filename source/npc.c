#include <stdio.h>
#include <string.h>

#include "npc.h"

static NPC world_npcs[] = {
    {
        1,
        REGION_KANTO,
        5,
        3,
        "Mira",
        "Hello! Welcome to the world of Pokemon!",
        true
    },
    {
        2,
        REGION_KANTO,
        10,
        7,
        "Researcher",
        "I am studying Pokemon from different regions.",
        true
    },
    {
        3,
        REGION_JOHTO,
        7,
        4,
        "Trainer",
        "Johto is full of wonderful Pokemon!",
        true
    },
    {
        4,
        REGION_HOENN,
        12,
        7,
        "Explorer",
        "The ocean hides many secrets.",
        true
    }
};

static const unsigned int npc_count =
    sizeof(world_npcs) / sizeof(world_npcs[0]);

void npc_init(void)
{
    for (unsigned int i = 0; i < npc_count; i++)
    {
        world_npcs[i].active = true;
    }
}

void npc_update(Game *game)
{
    (void)game;
}

void npc_render(Game *game)
{
    (void)game;
}

bool npc_interact(Game *game)
{
    if (game == NULL)
        return false;

    int px = game->player.x;
    int py = game->player.y;

    for (unsigned int i = 0; i < npc_count; i++)
    {
        NPC *npc = &world_npcs[i];

        if (!npc->active ||
            npc->region != game->player.region)
            continue;

        int dx = npc->x - px;
        int dy = npc->y - py;

        if ((dx == 0 && dy == 0) ||
            (dx == 0 && (dy == 1 || dy == -1)) ||
            (dy == 0 && (dx == 1 || dx == -1)))
        {
            snprintf(
                game->notice,
                sizeof(game->notice),
                "%s: %s",
                npc->name,
                npc->dialogue
            );

            return true;
        }
    }

    snprintf(
        game->notice,
        sizeof(game->notice),
        "There is nobody nearby."
    );

    return false;
}

const NPC *npc_get_list(void)
{
    return world_npcs;
}

unsigned int npc_get_count(void)
{
    return npc_count;
}
