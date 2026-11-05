
#include <3ds.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAP_W 20
#define MAP_H 12
#define SAVE_PATH "sdmc:/pokemon_world_save.dat"
#define SAVE_MAGIC 0x50574C44u
#define SAVE_VERSION 1u

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    SCREEN_WORLD,
    SCREEN_MAIN,
    SCREEN_BAG,
    SCREEN_PAUSE,
    SCREEN_POKEDEX
} Screen;

typedef struct {
    uint32_t magic;
    uint32_t version;
    int32_t x;
    int32_t y;
    int32_t region;
} SaveData;

typedef struct {
    int x;
    int y;
    const char *name;
    const char *dialogue;
} NPC;

static const char *regionNames[] = {
    "KANTO", "JOHTO", "HOENN", "SINNOH",
    "UNOVA", "KALOS", "ALOLA"
};

static const char *mapRows[MAP_H] = {
    "####################",
    "#......####........#",
    "#......####........#",
    "#..................#",
    "#....H.............#",
    "#..................#",
    "#.............P....#",
    "#..................#",
    "#..####............#",
    "#..####............#",
    "#..................#",
    "####################"
};

/* NPC SYSTEM */
static const NPC npcs[] = {
    {
        10, 5,
        "Mira",
        "Hello, Trainer! Welcome to Pokemon World!"
    },
    {
        4, 7,
        "Youngster",
        "I want to become a Pokemon Champion!"
    },
    {
        15, 3,
        "Researcher",
        "There are seven regions waiting to be explored."
    }
};

#define NPC_COUNT (sizeof(npcs) / sizeof(npcs[0]))

static const char *mainItems[] = {
    "Pokemon",
    "Pokedex",
    "Bag",
    "Trainer Card",
    "Back"
};

static const char *bagItems[] = {
    "Items",
    "Medicine",
    "Poke Balls",
    "TMs",
    "Key Items",
    "Save Game",
    "Load Game",
    "Back"
};

static const char *pauseItems[] = {
    "Resume",
    "Bag",
    "Pokedex",
    "Back"
};

static bool isNpcAt(int x, int y) {
    for (int i = 0; i < NPC_COUNT; i++) {
        if (npcs[i].x == x && npcs[i].y == y)
            return true;
    }

    return false;
}

static bool isWalkable(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H)
        return false;

    if (mapRows[y][x] != '.')
        return false;

    if (isNpcAt(x, y))
        return false;

    return true;
}

static bool saveGame(int x, int y, int region) {
    SaveData data = {
        SAVE_MAGIC, SAVE_VERSION, x, y, region
    };

    FILE *file = fopen(SAVE_PATH, "wb");
    if (!file) return false;

    bool ok = fwrite(&data, sizeof(data), 1, file) == 1;

    if (fclose(file) != 0)
        ok = false;

    return ok;
}

static bool loadGame(int *x, int *y, int *region) {
    SaveData data;

    FILE *file = fopen(SAVE_PATH, "rb");
    if (!file) return false;

    bool ok = fread(&data, sizeof(data), 1, file) == 1;
    fclose(file);

    if (!ok ||
        data.magic != SAVE_MAGIC ||
        data.version != SAVE_VERSION)
        return false;

    if (data.x < 1 || data.x >= MAP_W - 1 ||
        data.y < 1 || data.y >= MAP_H - 1 ||
        data.region < 0 || data.region >= 7 ||
        !isWalkable(data.x, data.y))
        return false;

    *x = data.x;
    *y = data.y;
    *region = data.region;

    return true;
}

static void interact(
    int x,
    int y,
    Direction dir,
    char *notice
) {
    int tx = x;
    int ty = y;

    switch (dir) {
        case DIR_UP:    ty--; break;
        case DIR_DOWN:  ty++; break;
        case DIR_LEFT:  tx--; break;
        case DIR_RIGHT: tx++; break;
    }

    if (tx < 0 || tx >= MAP_W ||
        ty < 0 || ty >= MAP_H) {
        snprintf(notice, 100, "Nothing in front.");
        return;
    }

    /* Check NPCs */
    for (int i = 0; i < NPC_COUNT; i++) {
        if (npcs[i].x == tx && npcs[i].y == ty) {
            snprintf(
                notice,
                100,
                "%s: %s",
                npcs[i].name,
                npcs[i].dialogue
            );
            return;
        }
    }

    char target = mapRows[ty][tx];

    if (target == 'H') {
        snprintf(notice, 100, "You found a House!");
    }
    else if (target == 'P') {
        snprintf(notice, 100, "Welcome to Pokemon Center!");
    }
    else {
        snprintf(notice, 100, "Nothing in front.");
    }
}

static void drawMap(int x, int y, int region) {
    consoleClear();

    printf("POKEMON WORLD\n");
    printf("Region: %s\n\n", regionNames[region]);

    for (int row = 0; row < MAP_H; row++) {
        for (int col = 0; col < MAP_W; col++) {
            if (col == x && row == y) {
                putchar('@');
            }
            else if (isNpcAt(col, row)) {
                putchar('N');
            }
            else {
                putchar(mapRows[row][col]);
            }
        }

        putchar('\n');
    }

    printf("\n@ Player  H House");
    printf("\nP Pokemon Center  N NPC");
    printf("\n\nD-Pad: Move");
    printf("\nA: Interact  B: Back");
    printf("\nX: Main Menu  Y: Bag");
    printf("\nSTART: Pause  SELECT: Pokedex");
}

static void drawMenu(
    const char *title,
    const char **items,
    int count,
    int selected,
    const char *notice
) {
    consoleClear();

    printf("POKEMON WORLD\n");
    printf("====================\n");
    printf("%s\n\n", title);

    for (int i = 0; i < count; i++) {
        printf("%s %s\n",
            i == selected ? ">" : " ",
            items[i]);
    }

    printf("\nD-Pad: Select");
    printf("\nA: Confirm  B: Back");

    if (notice && notice[0] != '\0')
        printf("\n\n%s", notice);
}

static void moveSelection(int *selected, int count, u32 down) {
    if (down & KEY_UP)
        *selected = (*selected + count - 1) % count;

    if (down & KEY_DOWN)
        *selected = (*selected + 1) % count;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    int playerX = 2;
    int playerY = 3;
    int region = 0;
    int selected = 0;

    Direction playerDir = DIR_DOWN;
    Screen screen = SCREEN_WORLD;

    char notice[100] = "Welcome to Pokemon World!";

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();

        if ((down & KEY_START) && screen == SCREEN_WORLD) {
            screen = SCREEN_PAUSE;
            selected = 0;
        }

        switch (screen) {

        case SCREEN_WORLD:

            if (down & KEY_UP) {
                playerDir = DIR_UP;
                if (isWalkable(playerX, playerY - 1))
                    playerY--;
            }
            else if (down & KEY_DOWN) {
                playerDir = DIR_DOWN;
                if (isWalkable(playerX, playerY + 1))
                    playerY++;
            }
            else if (down & KEY_LEFT) {
                playerDir = DIR_LEFT;
                if (isWalkable(playerX - 1, playerY))
                    playerX--;
            }
            else if (down & KEY_RIGHT) {
                playerDir = DIR_RIGHT;
                if (isWalkable(playerX + 1, playerY))
                    playerX++;
            }

            if (down & KEY_A)
                interact(playerX, playerY, playerDir, notice);

            if (down & KEY_X) {
                screen = SCREEN_MAIN;
                selected = 0;
            }

            if (down & KEY_Y) {
                screen = SCREEN_BAG;
                selected = 0;
            }

            if (down & KEY_SELECT)
                screen = SCREEN_POKEDEX;

            drawMap(playerX, playerY, region);
            printf("\n\nStatus: %s", notice);
            break;

        case SCREEN_MAIN:

            moveSelection(&selected, 5, down);

            if (down & KEY_A) {
                if (selected == 1) {
                    screen = SCREEN_POKEDEX;
                }
                else if (selected == 2) {
                    screen = SCREEN_BAG;
                    selected = 0;
                }
                else if (selected == 4) {
                    screen = SCREEN_WORLD;
                }
                else {
                    snprintf(notice, 100, "Coming soon!");
                }
            }

            if (down & KEY_B)
                screen = SCREEN_WORLD;

            drawMenu("MAIN MENU", mainItems, 5,
                     selected, notice);
            break;

        case SCREEN_BAG:

            moveSelection(&selected, 8, down);

            if (down & KEY_A) {
                if (selected == 5) {
                    snprintf(notice, 100, "%s",
                        saveGame(playerX, playerY, region)
                        ? "Save successful."
                        : "Save failed.");
                }
                else if (selected == 6) {
                    snprintf(notice, 100, "%s",
                        loadGame(&playerX, &playerY, &region)
                        ? "Load successful."
                        : "Load failed.");
                }
                else if (selected == 7) {
                    screen = SCREEN_WORLD;
                }
                else {
                    snprintf(notice, 100, "Coming soon!");
                }
            }

            if (down & KEY_B)
                screen = SCREEN_WORLD;

            drawMenu("BAG", bagItems, 8,
                     selected, notice);
            break;

        case SCREEN_PAUSE:

            moveSelection(&selected, 4, down);

            if (down & KEY_A) {
                if (selected == 0 || selected == 3) {
                    screen = SCREEN_WORLD;
                }
                else if (selected == 1) {
                    screen = SCREEN_BAG;
                    selected = 0;
                }
                else if (selected == 2) {
                    screen = SCREEN_POKEDEX;
                }
            }

            if (down & KEY_B)
                screen = SCREEN_WORLD;

            drawMenu("PAUSE MENU", pauseItems, 4,
                     selected, notice);
            break;

        case SCREEN_POKEDEX:

            consoleClear();

            printf("POKEMON WORLD\n");
            printf("====================\n");
            printf("POKEDEX\n\n");
            printf("Pokemon discovered: 0\n");
            printf("Pokemon caught: 0\n");
            printf("\nPokedex system coming soon.");
            printf("\n\nA or B: Back");

            if (down & (KEY_A | KEY_B))
                screen = SCREEN_WORLD;

            break;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
