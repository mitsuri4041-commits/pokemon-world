#include <stdio.h>
#include <string.h>

#include "quest.h"

void quest_init(Game *game)
{
    if (game == NULL)
        return;

    memset(&game->current_quest, 0,
           sizeof(game->current_quest));

    game->current_quest.quest_id = 1;

    snprintf(game->current_quest.title,
             sizeof(game->current_quest.title),
             "A New Beginning");

    snprintf(game->current_quest.description,
             sizeof(game->current_quest.description),
             "Talk to Mira in Kanto.");
}

void quest_start(Game *game, uint16_t quest_id)
{
    if (game == NULL || quest_id == 0)
        return;

    game->current_quest.quest_id = quest_id;
    game->current_quest.active = true;
    game->current_quest.completed = false;

    snprintf(game->notice, sizeof(game->notice),
             "Quest started!");
}

void quest_update(Game *game)
{
    if (game == NULL)
        return;

    if (game->current_quest.active &&
        game->current_quest.quest_id == 1 &&
        game->player.x == 5 &&
        game->player.y == 3)
    {
        quest_complete(game, 1);
    }
}

void quest_complete(Game *game, uint16_t quest_id)
{
    if (game == NULL ||
        game->current_quest.quest_id != quest_id ||
        game->current_quest.completed)
        return;

    game->current_quest.active = false;
    game->current_quest.completed = true;

    snprintf(game->notice, sizeof(game->notice),
             "Quest completed! Great work!");
}

bool quest_is_completed(Game *game, uint16_t quest_id)
{
    if (game == NULL)
        return false;

    return game->current_quest.quest_id == quest_id &&
           game->current_quest.completed;
}
