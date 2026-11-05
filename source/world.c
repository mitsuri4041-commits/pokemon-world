#include <stdio.h>

#include "world.h"
#include "npc.h"
#include "player.h"

static const char *kanto_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#..T...............#",
    "#...........T......#",
    "#......####........#",
    "#..................#",
    "#....H.............#",
    "#..................#",
    "#............P.....#",
    "#..................#",
    "#..................#",
    "####################"
};

static const char *johto_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#....T.............#",
    "#..................#",
    "#.........####.....#",
    "#..................#",
    "#......H...........#",
    "#..................#",
    "#..............P...#",
    "#..................#",
    "#..................#",
    "####################"
};

static const char *hoenn_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#..T...............#",
    "#..................#",
    "#......~~~~........#",
    "#......~~~~........#",
    "#..................#",
    "#..........T.......#",
    "#..............P...#",
    "#..................#",
    "#..................#",
    "####################"
};

static const char *sinnoh_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#......T...........#",
    "#..................#",
    "#....####..........#",
    "#..................#",
    "#..............H...#",
    "#..................#",
    "#..............P...#",
    "#..................#",
    "#..................#",
    "####################"
};

static const char *unova_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#..T...............#",
    "#..................#",
    "#...........T......#",
    "#..................#",
    "#......H...........#",
    "#..................#",
    "#..............P...#",
    "#..................#",
    "#..................#",
    "####################"
};

static const char *kalos_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#.........T........#",
    "#..................#",
    "#....####..........#",
    "#..................#",
    "#......H...........#",
    "#..................#",
    "#..............P...#",
    "#..................#",
    "#..................#",
    "####################"
};

static const char *alola_map[MAP_HEIGHT] = {
    "####################",
    "#..................#",
    "#..T...............#",
    "#..................#",
    "#......~~~~........#",
    "#......~~~~........#",
    "#..................#",
    "#...........T......#",
    "#..............P...#",
    "#..................#",
    "#..................#",
    "####################"
};

static const Region regions[REGION_COUNT] = {
    { REGION_KANTO,  "Kanto",  kanto_map },
    { REGION_JOHTO,  "Johto",  johto_map },
    { REGION_HOENN,  "Hoenn",  hoenn_map },
    { REGION_SINNOH, "Sinnoh", sinnoh_map },
    { REGION_UNOVA,  "Unova",  unova_map },
    { REGION_KALOS,  "Kalos",  kalos_map },
    { REGION_ALOLA,  "Alola",  alola_map }
};

const Region *world_get_region(RegionID region)
{
    if (region < 0 || region >= REGION_COUNT)
        return NULL;

    return &regions[region];
}

bool world_is_walkable(RegionID region, int x, int y)
{
    const Region *area = world_get_region(region);

    if (area == NULL)
        return false;

    if (x < 0 || x >= MAP_WIDTH ||
        y < 0 || y >= MAP_HEIGHT)
        return false;

    return area->map[y][x] == '.';
}

bool world_is_npc_at(RegionID region, int x, int y)
{
    const NPC *npcs = npc_get_list();
    unsigned int count = npc_get_count();

    for (unsigned int i = 0; i < count; i++)
    {
        if (npcs[i].active &&
            npcs[i].region == region &&
            npcs[i].x == x &&
            npcs[i].y == y)
        {
            return true;
        }
    }

    return false;
}

void world_init(Game *game)
{
    if (game == NULL)
        return;

    game->player.x = 1;
    game->player.y = 1;
    game->player.region = REGION_KANTO;
}

void world_update(Game *game)
{
    if (game == NULL)
        return;

    player_update(game);
}

void world_render(Game *game)
{
    if (game == NULL)
        return;

    const Region *area =
        world_get_region(game->player.region);

    if (area == NULL)
        return;

    printf("\x1b[2J");
    printf("\x1b[1;1H");

    printf("========== POKEMON WORLD ==========\n");
    printf("Region: %s\n", area->name);
    printf("===================================\n");

    const NPC *npcs = npc_get_list();
    unsigned int npc_count = npc_get_count();

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            char tile = area->map[y][x];

            if (game->player.x == x &&
                game->player.y == y)
            {
                printf("@");
                continue;
            }

            bool npc_found = false;

            for (unsigned int i = 0; i < npc_count; i++)
            {
                if (npcs[i].active &&
                    npcs[i].region == game->player.region &&
                    npcs[i].x == x &&
                    npcs[i].y == y)
                {
                    printf("N");
                    npc_found = true;
                    break;
                }
            }

            if (!npc_found)
                printf("%c", tile);
        }

        printf("\n");
    }

    printf("\n");
    printf("@ Player  N NPC  T Tree");
    printf("\nH House  P Pokemon Center");
    printf("\nD-Pad: Move | A: Interact");
}
