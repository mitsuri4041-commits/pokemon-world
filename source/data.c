#include "data.h"

static const PokemonData pokemon_data[] = {
    { 1,   "Bulbasaur",  45, 49, 49, 45, 12, 3 },
    { 2,   "Ivysaur",    60, 62, 63, 60, 12, 3 },
    { 3,   "Venusaur",   80, 82, 83, 80, 12, 3 },
    { 4,   "Charmander", 39, 52, 43, 65, 10, 0 },
    { 5,   "Charmeleon", 58, 64, 58, 80, 10, 0 },
    { 6,   "Charizard",  78, 84, 78, 100, 10, 2 },
    { 7,   "Squirtle",   44, 48, 65, 43, 11, 0 },
    { 8,   "Wartortle",  59, 63, 80, 58, 11, 0 },
    { 9,   "Blastoise",  79, 83, 100, 78, 11, 0 },
    { 25,  "Pikachu",    35, 55, 40, 90, 13, 0 },
    { 26,  "Raichu",     60, 90, 55, 110, 13, 0 },
    { 133, "Eevee",      55, 55, 50, 55, 1, 0 },
    { 150, "Mewtwo",     106, 110, 90, 130, 14, 0 },
    { 151, "Mew",        100, 100, 100, 100, 14, 0 }
};

const PokemonData *data_get_pokemon(uint16_t species)
{
    unsigned int count =
        sizeof(pokemon_data) / sizeof(pokemon_data[0]);

    for (unsigned int i = 0; i < count; i++)
    {
        if (pokemon_data[i].national_dex == species)
            return &pokemon_data[i];
    }

    return 0;
}

const char *data_get_region_name(RegionID region)
{
    static const char *names[REGION_COUNT] = {
        "Kanto",
        "Johto",
        "Hoenn",
        "Sinnoh",
        "Unova",
        "Kalos",
        "Alola"
    };

    if (region >= REGION_COUNT)
        return "Unknown";

    return names[region];
}
