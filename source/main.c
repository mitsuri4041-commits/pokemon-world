#include <3ds.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAP_W 20
#define MAP_H 12
#define PARTY_SIZE 6
#define SAVE_PATH "sdmc:/pokemon_world_save.dat"

typedef enum {
    KANTO, JOHTO, HOENN, SINNOH,
    UNOVA, KALOS, ALOLA, REGION_COUNT
} Region;

typedef struct {
    uint16_t species;
    uint16_t level;
    uint32_t exp;
    uint16_t hp;
    uint16_t max_hp;
    char name[32];
    bool shiny;
} Pokemon;

typedef struct {
    int x, y;
    Region region;
    Pokemon party[PARTY_SIZE];
    int party_count;
    uint32_t money;
} Player;

typedef struct {
    int x, y;
    Region region;
    char name[32];
    char dialogue[128];
} NPC;

typedef struct {
    bool active;
    bool completed;
    int id;
    char title[64];
} Quest;

typedef struct {
    uint32_t magic;
    Player player;
    Quest quest;
} SaveData;

typedef struct {
    Player player;
    Quest quest;
    bool running;
    char notice[128];
} Game;

/* =================================
   REGION DATA
================================= */

static const char *region_names[REGION_COUNT] = {
    "Kanto", "Johto", "Hoenn", "Sinnoh",
    "Unova", "Kalos", "Alola"
};

static const char *maps[REGION_COUNT][MAP_H] = {
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    }
};

/* =================================
   NPC DATA
================================= */

static NPC npcs[] = {
    {5, 3, KANTO, "Mira",
     "Welcome to Pokemon World!"},

    {10, 7, KANTO, "Researcher",
     "Explore all seven regions!"},

    {7, 4, JOHTO, "Trainer",
     "Johto has many Pokemon!"},

    {12, 7, HOENN, "Explorer",
     "The ocean hides secrets!"},

    {8, 5, SINNOH, "Scientist",
     "Keep discovering Pokemon!"},

    {6, 6, UNOVA, "Trainer",
     "Your adventure continues!"},

    {9, 3, KALOS, "Traveler",
     "Welcome to Kalos!"},

    {8, 7, ALOLA, "Island Guide",
     "Enjoy your island adventure!"}
};

#define NPC_COUNT (sizeof(npcs) / sizeof(npcs[0]))

/* =================================
   PLAYER SYSTEM
================================= */

static void player_init(Player *p)
{
    memset(p, 0, sizeof(Player));

    p->x = 1;
    p->y = 1;
    p->region = KANTO;
    p->money = 3000;
}

static bool player_has_space(Player *p)
{
    return p->party_count < PARTY_SIZE;
}

static void player_add_pokemon(
    Player *p,
    uint16_t species,
    uint16_t level,
    const char *name)
{
    if (!player_has_space(p))
        return;

    Pokemon *pk = &p->party[p->party_count];

    memset(pk, 0, sizeof(Pokemon));

    pk->species = species;
    pk->level = level;
    pk->hp = 10;
    pk->max_hp = 10;

    snprintf(pk->name, sizeof(pk->name), "%s", name);

    p->party_count++;
}

/* =================================
   WORLD SYSTEM
================================= */

static bool world_walkable(
    Region region,
    int x,
    int y)
{
    if (region >= REGION_COUNT)
        return false;

    if (x < 0 || x >= MAP_W ||
        y < 0 || y >= MAP_H)
        return false;

    return maps[region][y][x] == '.';
}

static bool npc_at(
    Region region,
    int x,
    int y)
{
    for (unsigned int i = 0; i < NPC_COUNT; i++)
    {
        if (npcs[i].region == region &&
            npcs[i].x == x &&
            npcs[i].y == y)
            return true;
    }

    return false;
}

static void player_move(Game *g, int dx, int dy)
{
    int nx = g->player.x + dx;
    int ny = g->player.y + dy;

    if (!world_walkable(
        g->player.region, nx, ny))
        return;

    if (npc_at(g->player.region, nx, ny))
        return;

    g->player.x = nx;
    g->player.y = ny;
}

/* =================================
   NPC INTERACTION
================================= */

static void npc_interact(Game *g)
{
    for (unsigned int i = 0; i < NPC_COUNT; i++)
    {
        NPC *n = &npcs[i];

        if (n->region != g->player.region)
            continue;

        int dx = n->x - g->player.x;
        int dy = n->y - g->player.y;

        if ((dx == 0 && dy == 0) ||
            (dx == 0 && (dy == 1 || dy == -1)) ||
            (dy == 0 && (dx == 1 || dx == -1)))
        {
            snprintf(
                g->notice,
                sizeof(g->notice),
                "%s: %s",
                n->name,
                n->dialogue
            );

            if (n->region == KANTO &&
                n->x == 5 && n->y == 3 &&
                !g->quest.completed)
            {
                g->quest.completed = true;
                g->quest.active = false;

                strncat(
                    g->notice,
                    " Quest completed!",
                    sizeof(g->notice) -
                    strlen(g->notice) - 1
                );
            }

            return;
        }
    }

    snprintf(g->notice, sizeof(g->notice),
             "Nobody is nearby.");
}

/* =================================
   QUEST SYSTEM
================================= */

static void quest_init(Quest *q)
{
    memset(q, 0, sizeof(Quest));

    q->id = 1;
    q->active = true;

    snprintf(q->title, sizeof(q->title),
             "A New Beginning");
}

/* =================================
   SAVE SYSTEM
================================= */

static bool save_game(Game *g)
{
    SaveData data;

    memset(&data, 0, sizeof(data));

    data.magic = 0x504F4B45;
    data.player = g->player;
    data.quest = g->quest;

    FILE *f = fopen(SAVE_PATH, "wb");

    if (!f)
        return false;

    bool success =
        fwrite(&data, sizeof(data), 1, f) == 1;

    fclose(f);

    return success;
}

static bool load_game(Game *g)
{
    SaveData data;

    FILE *f = fopen(SAVE_PATH, "rb");

    if (!f)
        return false;

    bool success =
        fread(&data, sizeof(data), 1, f) == 1;

    fclose(f);

    if (!success || data.magic != 0x504F4B45)
        return false;

    if (data.player.region >= REGION_COUNT ||
        data.player.party_count < 0 ||
        data.player.party_count > PARTY_SIZE)
        return false;

    g->player = data.player;
    g->quest = data.quest;

    return true;
}

/* =================================
   GAME INITIALIZATION
================================= */

static void game_init(Game *g)
{
    memset(g, 0, sizeof(Game));

    g->running = true;

    player_init(&g->player);
    quest_init(&g->quest);

    /* Starter Pokemon */
    player_add_pokemon(
        &g->player, 25, 5, "Pikachu"
    );

    snprintf(
        g->notice,
        sizeof(g->notice),
        "Welcome to Pokemon World!"
    );

    load_game(g);
}

/* =================================
   GAME UPDATE
================================= */

static void game_update(Game *g)
{
    u32 keys = hidKeysDown();

    if (keys & KEY_UP)
        player_move(g, 0, -1);

    if (keys & KEY_DOWN)
        player_move(g, 0, 1);

    if (keys & KEY_LEFT)
        player_move(g, -1, 0);

    if (keys & KEY_RIGHT)
        player_move(g, 1, 0);

    if (keys & KEY_A)
        npc_interact(g);

    if (keys & KEY_START)
    {
        if (save_game(g))
            snprintf(g->notice, sizeof(g->notice),
                     "Game saved!");
        else
            snprintf(g->notice, sizeof(g->notice),
                     "Save failed!");
    }

    if (keys & KEY_SELECT)
    {
        if (load_game(g))
            snprintf(g->notice, sizeof(g->notice),
                     "Game loaded!");
        else
            snprintf(g->notice, sizeof(g->notice),
                     "No save data!");
    }

    if (keys & KEY_B)
        g->running = false;
}

/* =================================
   GAME RENDER
================================= */

static void game_render(Game *g)
{
    consoleClear();

    printf("================================\n");
    printf("         POKEMON WORLD\n");
    printf("================================\n");

    printf("Region: %s\n",
           region_names[g->player.region]);

    printf("Money: %lu\n",
           (unsigned long)g->player.money);

    printf("Party: %d/%d\n\n",
           g->player.party_count, PARTY_SIZE);

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if (g->player.x == x &&
                g->player.y == y)
            {
                printf("@");
                continue;
            }

            if (npc_at(g->player.region, x, y))
            {
                printf("N");
                continue;
            }

            printf("%c",
                maps[g->player.region][y][x]);
        }

        printf("\n");
    }

    printf("\n@ Player  N NPC  T Tree");
    printf("\nH House  P Pokemon Center");

    printf("\n\nQuest: %s",
           g->quest.title);

    printf("\nStatus: %s",
           g->quest.completed ?
           "Completed" : "Active");

    printf("\n\nNOTICE: %s", g->notice);

    printf("\n\nD-Pad: Move");
    printf("\nA: Talk");
    printf("\nSTART: Save");
    printf("\nSELECT: Load");
    printf("\nB: Exit");
}

/* =================================
   MAIN
================================= */

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    gfxInitDefault();

    consoleInit(GFX_TOP, NULL);

    Game game;

    game_init(&game);

    while (aptMainLoop() && game.running)
    {
        hidScanInput();

        game_update(&game);
        game_render(&game);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    save_game(&game);

    gfxExit();

    return 0;
}
