#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

void player_init(Player *player);
void player_update(Game *game);
void player_move(Game *game, int dx, int dy);

void player_add_pokemon(
    Player *player,
    uint16_t species,
    uint16_t level
);

bool player_has_space(const Player *player);

#endif
