#ifndef QUEST_H
#define QUEST_H

#include "game.h"

#define MAX_QUESTS 32

typedef enum {
    QUEST_NOT_STARTED = 0,
    QUEST_ACTIVE,
    QUEST_COMPLETED
} QuestStatus;

typedef struct {
    uint16_t id;

    char title[MAX_NAME_LENGTH];
    char description[MAX_NOTICE_LENGTH];

    QuestStatus status;

    uint8_t required_amount;
    uint8_t current_amount;
} QuestData;

void quest_init(Game *game);
void quest_update(Game *game);
void quest_start(Game *game, uint16_t quest_id);
void quest_complete(Game *game, uint16_t quest_id);

bool quest_is_completed(Game *game, uint16_t quest_id);

#endif
