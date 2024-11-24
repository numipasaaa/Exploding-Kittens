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

int search_card(Card* hand, int hand_size, CardType type)
{
    for (int i = 0; i < hand_size; i++)
    {
        if (hand[i].type == type)
        {
            return i;
        }
    }
    return -1;
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

    for (int i = 0; i < MAX_PLAYERS - 1; i++) {
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
  	time_t curtime;
    time(&curtime);
    srand(curtime);

    game->current_player_index = rand() % 2;
    game->num_players = 0;
    game->deck_index = 45;
    game->pile_index = 0;
    game->game_over = false;
    game->attack_stack = 0;
    game->skip_active = false;
    game->safe_from_attack = false;
    init_deck(game->deck);
}

void use_defuse(Game* game, int player_id, int card_index)
{
    for (int i = card_index; i < game->players[player_id].num_cards - 1; i++)
    {
        game->players[player_id].hand[i] = game->players[player_id].hand[i + 1];
        game->players[player_id].num_cards--;
    }
}

void reinsert_exploding_kitten(Game* game, int player_id)
{
    int index;

   	printf("Index to reinsert(0-%d): ", game->deck_index);
    scanf("%d", &index);

    for (int i = game->deck_index; i > index; i--)
    {
        game->deck[i] = game->deck[i - 1];
    }

    game->deck[index].type = EXPLODING_KITTEN;
}

void handle_explosion(Game* game, int player_id)
{
    int defuse_index = search_card(game->players[player_id].hand, game->players[player_id].num_cards, DEFUSE);
    if (defuse_index != -1)
    {
        use_defuse(game, player_id, defuse_index);
        reinsert_exploding_kitten(game, player_id);

    }
    else
    {
        game->num_players--;
    }
}

void draw_card(Game* game, int player_id)
{
  	if (game->deck[game->deck_index].type == EXPLODING_KITTEN)
    {
        printf("EXPLODING KITTEN!\n");
        handle_explosion(game, player_id);
    }
    else
    {
    	game->players[player_id].hand[game->players[player_id].num_cards] = game->deck[game->deck_index];
    	game->players[player_id].num_cards++;
    	game->deck_index--;
    }
}

void play_card(Game* game, int player_id, int card_index)
{
    int next_player_idx;
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
            game->skip_active = true;
            game->safe_from_attack = true;
        	break;
        case SKIP:
        	if (game->attack_stack > 0) {
            	game->attack_stack--;
        	}
            game->skip_active = true;
        	break;
        case SHUFFLE:
            next_player_idx = (player_id == 0) ? 1 : 0;
        	shuffle_deck(game->deck, game->deck_index + 1);
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
          	printf("Play card: ");
            scanf("%d", &card_index);
            play_card(game, player_id, card_index);
            break;
        case DRAW_CARD:
            draw_card(game, player_id);
            break;
        case END_TURN:
          	draw_card(game, player_id);

            if (game->attack_stack > 0) {
                game->attack_stack--;
            } else {
            	game->current_player_index = (player_id == 0) ? 1 : 0;
            }
            break;
        default:
            break;
    }
}

void handle_turn(Game* game)
{
    int player_id = game->current_player_index;
    Action action;

    do {
        printf("\nPlayer %d's turn\n", player_id + 1);
        printf("Hand:\n");
        print_cards(game->players[player_id].hand, game->players[player_id].num_cards);
        printf("Action:\n");
        printf("1. Play card\n");
        printf("2. End turn\n");
        printf("Choose action: ");
        scanf("%d", &action.type);
        handle_player_action(game, player_id, action);

        if (game->num_players == 1) {
            game->game_over = true;
            return;
        }

        if (game->skip_active && (game->attack_stack == 0 || game->safe_from_attack)) {
            action.type = END_TURN;
            handle_player_action(game, player_id, action);
            game->safe_from_attack = false;
            game->skip_active = false;
            break;
        } else if (game->skip_active) {
            game->skip_active = false;
            continue;
        }

        if (game->attack_stack > 0) {
            	game->attack_stack--;
        }
    } while (game->attack_stack > 0 || action.type == PLAY_CARD);
}


