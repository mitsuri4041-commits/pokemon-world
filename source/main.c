#include <3ds.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAP_W 20
#define MAP_H 12
#define SAVE_PATH "sdmc:/pokemon_world_save.dat"
#define SAVE_MAGIC 0x50574C44u /* PWLD */
#define SAVE_VERSION 1u

typedef struct {
    uint32_t magic;
    uint32_t version;
    int32_t x;
    int32_t y;
    int32_t region;
} SaveData;

static const char *regionNames[] = {
    "KANTO", "JOHTO", "HOENN", "SINNOH", "UNOVA", "KALOS", "ALOLA"
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

static bool isWalkable(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return false;
    return mapRows[y][x] != '#';
}

static void drawMap(int playerX, int playerY, int region) {
    consoleClear();
    printf("POKEMON WORLD - 3DS TEST\n");
    printf("Region: %s\n\n", regionNames[region]);
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (x == playerX && y == playerY) putchar('@');
            else putchar(mapRows[y][x]);
        }
        putchar('\n');
    }
    printf("\n@ You   H House   P Pokemon Center\n");
    printf("D-Pad: Move   A: Save   B: Load\n");
    printf("L/R: Change test region   START: Exit\n");
    printf("\nThis is a control/save proof-of-concept, not the full game yet.\n");
}

static bool saveGame(int x, int y, int region) {
    SaveData data = { SAVE_MAGIC, SAVE_VERSION, x, y, region };
    FILE *file = fopen(SAVE_PATH, "wb");
    if (!file) return false;
    bool ok = fwrite(&data, sizeof(data), 1, file) == 1;
    if (fclose(file) != 0) ok = false;
    return ok;
}

static bool loadGame(int *x, int *y, int *region) {
    SaveData data;
    FILE *file = fopen(SAVE_PATH, "rb");
    if (!file) return false;
    bool ok = fread(&data, sizeof(data), 1, file) == 1;
    fclose(file);
    if (!ok || data.magic != SAVE_MAGIC || data.version != SAVE_VERSION) return false;
    if (data.x < 1 || data.x >= MAP_W - 1 || data.y < 1 || data.y >= MAP_H - 1) return false;
    if (data.region < 0 || data.region >= 7 || !isWalkable(data.x, data.y)) return false;
    *x = data.x;
    *y = data.y;
    *region = data.region;
    return true;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    int playerX = 2;
    int playerY = 3;
    int region = 0;
    bool running = true;
    const char *notice = "Ready.";

    while (aptMainLoop() && running) {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_START) running = false;

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
            notice = saveGame(playerX, playerY, region)
                ? "Save successful."
                : "Save failed. Check SD card access.";
        }
        if (down & KEY_B) {
            notice = loadGame(&playerX, &playerY, &region)
                ? "Load successful."
                : "Load failed. No valid save found.";
        }
        if (down & KEY_L) region = (region + 6) % 7;
        if (down & KEY_R) region = (region + 1) % 7;

        drawMap(playerX, playerY, region);
        printf("\nStatus: %s\n", notice);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
