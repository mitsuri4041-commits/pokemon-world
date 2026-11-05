#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "save.h"

#define SAVE_MAGIC 0x504F4B45

typedef struct {
    uint32_t magic;
    uint32_t version;

    Player player;
    Quest quest;
} SaveData;

void save_init(void)
{
    /* Save system initialization */
}

bool save_game(const Game *game)
{
    if (game == NULL)
        return false;

    SaveData data;

    memset(&data, 0, sizeof(data));

    data.magic = SAVE_MAGIC;
    data.version = SAVE_VERSION;
    data.player = game->player;
    data.quest = game->current_quest;

    FILE *file = fopen(SAVE_FILE_PATH, "wb");

    if (file == NULL)
        return false;

    size_t written = fwrite(
        &data,
        sizeof(SaveData),
        1,
        file
    );

    fclose(file);

    return written == 1;
}

bool load_game(Game *game)
{
    if (game == NULL)
        return false;

    FILE *file = fopen(SAVE_FILE_PATH, "rb");

    if (file == NULL)
        return false;

    SaveData data;

    size_t read_count = fread(
        &data,
        sizeof(SaveData),
        1,
        file
    );

    fclose(file);

    if (read_count != 1)
        return false;

    if (data.magic != SAVE_MAGIC ||
        data.version != SAVE_VERSION)
        return false;

    game->player = data.player;
    game->current_quest = data.quest;

    snprintf(
        game->notice,
        sizeof(game->notice),
        "Game loaded successfully!"
    );

    return true;
}
