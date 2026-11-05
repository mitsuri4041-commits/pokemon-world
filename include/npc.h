#ifndef NPC_H
#define NPC_H

#include "game.h"

#define MAX_NPCS 16
#define MAX_DIALOGUE_LENGTH 128

typedef struct {
    int id;
    RegionID region;

    int x;
    int y;

    char name[MAX_NAME_LENGTH];

    const char *dialogue;
    bool active;
} NPC;

void npc_init(void);

void npc_update(Game *game);
void npc_render(Game *game);

bool npc_interact(Game *game);

const NPC *npc_get_list(void);
unsigned int npc_get_count(void);

#endif
