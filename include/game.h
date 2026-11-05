#ifndef GAME_H
#define GAME_H

#include <3ds.h>
#include <stdbool.h>
#include <stdint.h>

#define GAME_TITLE "POKEMON WORLD"

#define SCREEN_TOP_WIDTH 400
#define SCREEN_TOP_HEIGHT 240

#define SCREEN_BOTTOM_WIDTH 320
#define SCREEN_BOTTOM_HEIGHT 240

#define PARTY_SIZE 6
#define MAX_NAME_LENGTH 32
#define MAX_NOTICE_LENGTH 128

typedef enum {
    REGION_KANTO = 0,
    REGION_JOHTO,
    REGION_HOENN,
    REGION_SINNOH,
    REGION_UNOVA,
    REGION_KALOS,
    REGION_ALOLA,
    REGION_COUNT
} RegionID;

typedef struct {
    uint16_t species;
    uint16_t level;
    uint32_t experience;

    uint16_t hp;
    uint16_t max_hp;

    char nickname[MAX_NAME_LENGTH];

    bool is_shiny;
    bool is_egg;
} Pokemon;

typedef struct {
    int x;
    int y;

    RegionID region;

    Pokemon party[PARTY_SIZE];
    int party_count;

    uint32_t money;
    uint32_t play_time;
} Player;

typedef struct {
    bool active;
    bool completed;

    uint16_t quest_id;

    char title[MAX_NAME_LENGTH];
    char description[MAX_NOTICE_LENGTH];
} Quest;

typedef struct {
    Player player;

    Quest current_quest;

    bool running;
    bool menu_open;

    char notice[MAX_NOTICE_LENGTH];
} Game;

void game_init(Game *game);
void game_update(Game *game);
void game_render(Game *game);
void game_shutdown(Game *game);

#endif
