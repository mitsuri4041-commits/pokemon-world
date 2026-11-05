#include <stdio.h>
#include <string.h>

#include "player.h"
#include "world.h"
#include "npc.h"

void player_init(Player *player)
{
    if (player == NULL)
        return;

    memset(player, 0, sizeof(Player));

    player->x = 1;
    player->y = 1;
    player->region = REGION_KANTO;
    player->money = 3000;
    player->party_count = 0;
}

void player_move(Game *game, int dx, int dy)
{
    if (game == NULL)
        return;

    int new_x = game->player.x + dx;
    int new_y = game->player.y + dy;

    if (!world_is_walkable(
            game->player.region,
            new_x,
            new_y))
        return;

    if (world_is_npc_at(
            game->player.region,
            new_x,
            new_y))
        return;

    game->player.x = new_x;
    game->player.y = new_y;
}

void player_update(Game *game)
{
    if (game == NULL)
        return;

    u32 keys = hidKeysDown();

    if (keys & KEY_UP)
        player_move(game, 0, -1);

    else if (keys & KEY_DOWN)
        player_move(game, 0, 1);

    else if (keys & KEY_LEFT)
        player_move(game, -1, 0);

    else if (keys & KEY_RIGHT)
        player_move(game, 1, 0);

    if (keys & KEY_A)
        npc_interact(game);
}

bool player_has_space(const Player *player)
{
    if (player == NULL)
        return false;

    return player->party_count < PARTY_SIZE;
}

void player_add_pokemon(
    Player *player,
    uint16_t species,
    uint16_t level)
{
    if (player == NULL || !player_has_space(player))
        return;

    Pokemon *pokemon =
        &player->party[player->party_count];

    memset(pokemon, 0, sizeof(Pokemon));

    pokemon->species = species;
    pokemon->level = level;
    pokemon->hp = 10;
    pokemon->max_hp = 10;

    snprintf(
        pokemon->nickname,
        sizeof(pokemon->nickname),
        "Pokemon"
    );

    player->party_count++;
}
