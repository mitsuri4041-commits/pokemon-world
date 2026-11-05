#ifndef DATA_H
#define DATA_H

#include "game.h"

#define MAX_POKEMON_DATA 1000

typedef struct {
    uint16_t national_dex;
    const char *name;

    uint8_t base_hp;
    uint8_t base_attack;
    uint8_t base_defense;
    uint8_t base_speed;

    uint8_t type1;
    uint8_t type2;
} PokemonData;

const PokemonData *data_get_pokemon(uint16_t species);
const char *data_get_region_name(RegionID region);

#endif
