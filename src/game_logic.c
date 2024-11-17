#include "game_logic.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void init_deck(Card *deck) {
    for (int i = 0; i < 52; i++) {
        deck[i].type = i;
    }
}

void init_game(Game *game) {
    game->current_player = 0;
    game->num_players = 0;
    game->num_active = 0;
    game->game_over = false;
    init_deck(game->deck);
}

void handle_player_action(Game* game, int player_id, Action action)
{
    switch (action.type) {
        case PLAY_CARD:
            break;
        case DRAW_CARD:
            break;
        case END_TURN:
            break;
        default:
            break;
    }
}
