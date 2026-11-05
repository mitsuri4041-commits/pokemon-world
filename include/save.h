#ifndef SAVE_H
#define SAVE_H

#include "game.h"

#define SAVE_FILE_PATH "sdmc:/pokemon_world_save.dat"
#define SAVE_VERSION 2

bool save_game(const Game *game);
bool load_game(Game *game);
void save_init(void);

#endif
