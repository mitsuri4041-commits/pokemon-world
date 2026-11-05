#ifndef WORLD_H
#define WORLD_H

#include "game.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 12

typedef struct {
    RegionID id;
    const char *name;
    const char **map;
} Region;

void world_init(Game *game);
void world_update(Game *game);
void world_render(Game *game);

bool world_is_walkable(RegionID region, int x, int y);
bool world_is_npc_at(RegionID region, int x, int y);

const Region *world_get_region(RegionID region);

#endif
