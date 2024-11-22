#include "game_logic.h"

int main(void)
{
    Game game;

    init_game(&game);

    game.num_players = 2;

    init_players_hands(&game);

    for (int j = 0; j < game.num_players; j++)
    {
        printf("Player %d's hand:\n", j + 1);
        print_cards(game.players[j].hand, game.players[j].num_cards);
    }

    Action action;
    action.type = PLAY_CARD;
    handle_turn(&game, 0);

    return 0;
}
