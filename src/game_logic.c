#define _XOPEN_SOURCE
#include "game_logic.h"

void print_cards(Card* hand, int n)
{
	for (int i = 0; i < n; i++)
    {
    	if (n > 1)
        {
          	printf("Card %d: ", i);
   		}

        switch (hand[i].type)
        {
            case EXPLODING_KITTEN:
                printf("EXPLODING_KITTEN\n");
                break;
            case DEFUSE:
                printf("DEFUSE\n");
                break;
            case ATTACK:
                printf("ATTACK\n");
                break;
            case SKIP:
                printf("SKIP\n");
                break;
            case FAVOR:
                printf("FAVOR\n");
                break;
            case SHUFFLE:
                printf("SHUFFLE\n");
                break;
            case SEE_THE_FUTURE:
                printf("SEE_THE_FUTURE\n");
                break;
            case NOPE:
                printf("NOPE\n");
                break;
            case TACOCAT:
                printf("TACOCAT\n");
                break;
            case CATTERMELON:
                printf("CATTERMELON\n");
                break;
            case POTATO:
                printf("POTATO\n");
                break;
            case BEARD:
                printf("BEARD\n");
                break;
            case RAINBOW:
                printf("RAINBOW\n");
                break;
        }
    }
}

void shuffle_deck(Card *deck, int n) {
    time_t curtime;
    time(&curtime);
    srand(curtime);

    for (int i = n - 1; i >= 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[j];
        deck[j] = deck[i];
        deck[i] = temp;
    }
}

void init_players_hands(Game *game) {
    for (int i = 0; i < game->num_players; i++) {
        for (int j = 0; j < 7; j++) {
            game->players[i].hand[j] = game->deck[game->deck_index];
            game->deck_index--;
        }
        game->players[i].hand[7].type = DEFUSE;
        game->players[i].num_cards = 8;
    }

    for (int i = 0; i < MAX_PLAYERS - 1; i++) {        // 4x Exploding Kitten
        game->deck[++(game->deck_index)].type = EXPLODING_KITTEN;
    }
    for (int i = 0; i < 2; i++) {        // 2x Defuse
        game->deck[++(game->deck_index)].type = DEFUSE;
    }
    shuffle_deck(game->deck, game->deck_index + 1);
}

void init_deck(Card *deck) {
    for (int i = 0; i < 4; i++) {        // 4x Attack
        deck[i].type = ATTACK;
    }
    for (int i = 4; i < 8; i++) {        // 4x Skip
        deck[i].type = SKIP;
    }
    for (int i = 8; i < 12; i++) {        // 4x Favor
        deck[i].type = FAVOR;
    }
    for (int i = 12; i < 16; i++) {        // 4x Shuffle
        deck[i].type = SHUFFLE;
    }
    for (int i = 16; i < 21; i++) {       // 5x See the Future
        deck[i].type = SEE_THE_FUTURE;
    }
    for (int i = 21; i < 26; i++) {        // 5x Nope
        deck[i].type = NOPE;
    }
    for (int i = 26; i < 30; i++) {        // 4x Tacocat
        deck[i].type = TACOCAT;
    }
    for (int i = 30; i < 34; i++) {        // 4x Cattermelon
        deck[i].type = CATTERMELON;
    }
    for (int i = 34; i < 38; i++) {        // 4x Potato
        deck[i].type = POTATO;
    }
    for (int i = 38; i < 42; i++) {        // 4x Beard
        deck[i].type = BEARD;
    }
    for (int i = 42; i < 46; i++) {        // 4x Rainbow
        deck[i].type = RAINBOW;
    }
    shuffle_deck(deck, 46);
}

void init_game(Game *game) {
    game->current_player_index = 0;
    game->num_players = 0;
    game->deck_index = 45;
    game->pile_index = 0;
    game->game_over = false;
    game->attack_stack = 0;
    init_deck(game->deck);
}

void draw_card(Game* game, int player_id)
{
    game->players[player_id].hand[game->players[player_id].num_cards] = game->deck[game->deck_index];
    game->players[player_id].num_cards++;
    game->deck_index--;
}

void play_card(Game* game, int player_id, int card_index)
{
    game->pile[(game->pile_index)++] = game->players[player_id].hand[card_index];

    for (int i = card_index; i < game->players[player_id].num_cards - 1; i++) {
        game->players[player_id].hand[i] = game->players[player_id].hand[i + 1];
    }
    game->players[player_id].num_cards--;

    switch (game->pile[game->pile_index - 1].type) {
    	case SEE_THE_FUTURE:
        	printf("Future cards:\n");
        	printf("1. "); print_cards(&game->deck[game->deck_index], 1);
            printf("2. "); print_cards(&game->deck[game->deck_index - 1], 1);
            printf("3. "); print_cards(&game->deck[game->deck_index - 2], 1);
        	break;
    	case ATTACK:
        	game->attack_stack += 2;
        	break;
        case SKIP:
        	if (game->attack_stack > 0) {
            	game->attack_stack--;
        	}
        	break;
    	default:
        	break;
	}
}

void handle_player_action(Game* game, int player_id, Action action)
{
  	int card_index;
    switch (action.type) {
        case PLAY_CARD:
          	printf("Enter card index: ");
            scanf("%d", &card_index);
            play_card(game, player_id, card_index);
            break;
        case DRAW_CARD:
            break;
        case END_TURN:
            break;
        default:
            break;
    }
}

void handle_turn(Game* game, int player_id)
{
    Action action;
    action.type = PLAY_CARD;
    handle_player_action(game, player_id, action);

    if (game->num_players == 1) {
        game->game_over = true;
        return;
    }

    switch (game->pile[game->pile_index - 1].type) {
        case EXPLODING_KITTEN:
            break;
        case ATTACK:
            break;
        case NOPE:
            break;
        case TACOCAT:
            break;
        case CATTERMELON:
            break;
        case POTATO:
            break;
        case BEARD:
            break;
        case RAINBOW:
            break;
        default:
            break;
    }
}


