#include "game_logic.h"

int main(void)
{
    Game game;

    init_game(&game);

    game.num_players = 2;

    init_players_hands(&game);


    while (!game.game_over)
    {
        handle_turn(&game);
    }

    return 0;
}
