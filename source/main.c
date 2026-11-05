
#include <3ds.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAP_W 20
#define MAP_H 12
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;
#define SAVE_PATH "sdmc:/pokemon_world_save.dat"
#define SAVE_MAGIC 0x50574C44u
#define SAVE_VERSION 1u

typedef struct {
    uint32_t magic;
    uint32_t version;
    int32_t x;
    int32_t y;
    int32_t region;
} SaveData;

typedef enum {
    SCREEN_WORLD,
    SCREEN_MAIN,
    SCREEN_BAG,
    SCREEN_PAUSE,
    SCREEN_POKEDEX
} Screen;

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

static const char *mainItems[] = {
    "Pokemon",
    "Pokedex",
    "Bag",
    "Trainer Card",
    "Back"
};

static const char *pauseItems[] = {
    "Resume",
    "Bag",
    "Pokedex",
    "Back"
};

static bool isWalkable(int x, int y) {
    if (x < 0 || x >= MAP_W ||
        y < 0 || y >= MAP_H)
        return false;

    return mapRows[y][x] == '.';
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
        data.y < 1 || data.y >= MAP_H - 1)
        return false;

    if (data.region < 0 || data.region >= 7 ||
        !isWalkable(data.x, data.y))
        return false;

    *x = data.x;
    *y = data.y;
    *region = data.region;

    return true;
}

static void drawMenu(
    const char *title,
    const char **items,
    int count,
    int selected
) {
    consoleClear();

    printf("POKEMON WORLD\n");
    printf("====================\n");
    printf("%s\n\n", title);

    for (int i = 0; i < count; i++) {
        if (i == selected)
            printf(" > %s\n", items[i]);
        else
            printf("   %s\n", items[i]);
    }

    printf("\nD-Pad: Select");
    printf("\nA: Confirm");
    printf("\nB: Back");
}

static void drawWorld(
    int playerX,
    int playerY,
    int region,
    const char *notice
) {
    consoleClear();

    printf("POKEMON WORLD\n");
    printf("Region: %s\n\n", regionNames[region]);

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (x == playerX && y == playerY)
                putchar('@');
            else
                putchar(mapRows[y][x]);
        }
        putchar('\n');
    }

    printf("\n@ You  H House  P Pokemon Center\n");
    printf("\nA: Interact  B: Back");
    printf("\nX: Main Menu  Y: Bag");
    printf("\nSTART: Pause  SELECT: Pokedex");
    printf("\n\nStatus: %s\n", notice);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    int playerX = 2;
    int playerY = 3;
    Direction playerDir = DIR_DOWN;
    int region = 0;

    Screen screen = SCREEN_WORLD;
    Screen previousScreen = SCREEN_WORLD;

    int selected = 0;
    const char *notice = "Welcome to Pokemon World!";

    bool running = true;

    while (aptMainLoop() && running) {
        hidScanInput();

        u32 down = hidKeysDown();

        if (screen == SCREEN_WORLD) {

            int nextX = playerX;
            int nextY = playerY;

            if (down & KEY_UP) nextY--;
            if (down & KEY_DOWN) nextY++;
            if (down & KEY_LEFT) nextX--;
            if (down & KEY_RIGHT) nextX++;

            if (isWalkable(nextX, nextY)) {
                playerX = nextX;
                playerY = nextY;
            }

            if (down & KEY_A) {
                char tile = mapRows[playerY][playerX];

                if (tile == 'H')
                    notice = "You are near your house.";
                else if (tile == 'P')
                    notice = "Pokemon Center.";
                else
                    notice = "There is nothing here.";
            }

            if (down & KEY_X) {
                screen = SCREEN_MAIN;
                selected = 0;
            }

            if (down & KEY_Y) {
                screen = SCREEN_BAG;
                selected = 0;
            }

            if (down & KEY_START) {
                screen = SCREEN_PAUSE;
                selected = 0;
            }

            if (down & KEY_SELECT) {
                screen = SCREEN_POKEDEX;
            }
        }

        else if (screen == SCREEN_BAG) {

            if (down & KEY_UP)
                selected = (selected + 7) % 8;

            if (down & KEY_DOWN)
                selected = (selected + 1) % 8;

            if (down & KEY_B) {
                screen = SCREEN_WORLD;
            }

            if (down & KEY_A) {
                switch (selected) {
                    case 0:
                        notice = "Items section - coming soon.";
                        break;

                    case 1:
                        notice = "Medicine section - coming soon.";
                        break;

                    case 2:
                        notice = "Poke Balls section - coming soon.";
                        break;

                    case 3:
                        notice = "TMs section - coming soon.";
                        break;

                    case 4:
                        notice = "Key Items section - coming soon.";
                        break;

                    case 5:
                        notice = saveGame(playerX, playerY, region)
                            ? "Game saved successfully."
                            : "Save failed.";

                        break;

                    case 6:
                        notice = loadGame(
                            &playerX, &playerY, &region
                        ) ? "Game loaded successfully."
                          : "No valid save found.";

                        break;

                    case 7:
                        screen = SCREEN_WORLD;
                        break;
                }
            }
        }

        else if (screen == SCREEN_MAIN) {

            if (down & KEY_UP)
                selected = (selected + 4) % 5;

            if (down & KEY_DOWN)
                selected = (selected + 1) % 5;

            if (down & KEY_B)
                screen = SCREEN_WORLD;

            if (down & KEY_A) {
                switch (selected) {
                    case 0:
                        notice = "Pokemon Party - coming soon.";
                        break;

                    case 1:
                        screen = SCREEN_POKEDEX;
                        break;

                    case 2:
                        screen = SCREEN_BAG;
                        selected = 0;
                        break;

                    case 3:
                        notice = "Trainer Card - coming soon.";
                        break;

                    case 4:
                        screen = SCREEN_WORLD;
                        break;
                }
            }
        }

        else if (screen == SCREEN_PAUSE) {

            if (down & KEY_UP)
                selected = (selected + 3) % 4;

            if (down & KEY_DOWN)
                selected = (selected + 1) % 4;

            if (down & KEY_B)
                screen = SCREEN_WORLD;

            if (down & KEY_A) {
                switch (selected) {
                    case 0:
                        screen = SCREEN_WORLD;
                        break;

                    case 1:
                        screen = SCREEN_BAG;
                        selected = 0;
                        break;

                    case 2:
                        screen = SCREEN_POKEDEX;
                        break;

                    case 3:
                        screen = SCREEN_WORLD;
                        break;
                }
            }
        }

        else if (screen == SCREEN_POKEDEX) {
            if (down & KEY_B || down & KEY_A)
                screen = SCREEN_WORLD;
        }

        if (screen == SCREEN_WORLD) {
            drawWorld(
                playerX, playerY, region, notice
            );
        }

        else if (screen == SCREEN_BAG) {
            drawMenu(
                "BAG",
                bagItems,
                8,
                selected
            );
        }

        else if (screen == SCREEN_MAIN) {
            drawMenu(
                "MAIN MENU",
                mainItems,
                5,
                selected
            );
        }

        else if (screen == SCREEN_PAUSE) {
            drawMenu(
                "PAUSE MENU",
                pauseItems,
                4,
                selected
            );
        }

        else if (screen == SCREEN_POKEDEX) {
            consoleClear();
            printf("POKEDEX\n");
            printf("====================\n");
            printf("Pokedex is coming soon.\n");
            printf("\nA/B: Back");
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
