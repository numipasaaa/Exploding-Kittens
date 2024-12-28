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

int search_card(Card* hand, int hand_size, CardType type, int ignore_index)
{
    for (int i = 0; i < hand_size; i++)
    {
        if (hand[i].type == type && i != ignore_index)
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
    game->not_entered_special = true;
    init_deck(game->deck);
}

void use_defuse(Game* game, int player_id, int card_index)
{
    for (int i = card_index; i < game->players[player_id].num_cards - 1; i++)
    {
        game->players[player_id].hand[i] = game->players[player_id].hand[i + 1];
    }
    game->players[player_id].num_cards--;
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
    int defuse_index = search_card(game->players[player_id].hand, game->players[player_id].num_cards, DEFUSE, -1);
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

void draw_card_from_player(Game* game, int player_id, bool random)
{
	int next_player_idx = next_player_idx = (player_id == 0) ? 1 : 0;
    int card_index, card;

    if (random) {
      	card_index = rand() % game->players[next_player_idx].num_cards;
        printf("Card index to draw(0-%d): ", game->players[next_player_idx].num_cards - 1);
        scanf("%d", &card_index);
		game->players[player_id].hand[game->players[player_id].num_cards] = game->players[next_player_idx].hand[card_index];
    } else {
    	printf("Card to draw:\n");
        printf("1.Defuse\n2.Attack\n3.Skip\n4.Favor\n5.Shuffle\n6.See the Future\n7.Nope\n8.Tacocat\n9.Cattermelon\n10.Potato\n11.Beard\n12.Rainbow\n");
        printf("Choose card: ");
        scanf("%d", &card);

        card_index = search_card(game->players[next_player_idx].hand, game->players[next_player_idx].num_cards, card, -1);

        if (card_index == -1)
        {
            printf("Card not found\n");
            return;
        }

        game->players[player_id].hand[game->players[player_id].num_cards] = game->players[next_player_idx].hand[card_index];
    }

    for (int i = card_index; i < game->players[player_id].num_cards - 1; i++)
    {
        game->players[next_player_idx].hand[i] = game->players[next_player_idx].hand[i + 1];
    }

   	game->players[player_id].num_cards++;
    game->players[next_player_idx].num_cards--;
}

void handle_special_card(Game* game, int player_id, CardType card)
{
    int special_idx1 = search_card(game->players[player_id].hand, game->players[player_id].num_cards, card, -1);
    int special_idx2 = search_card(game->players[player_id].hand, game->players[player_id].num_cards, card, special_idx1);
    int combo = 0;

    if (game->not_entered_special)
    {
      	if (special_idx1 != -1)
        {
        	printf("Combos available:\n");
            printf("1. Two of a Kind\n");
            if (special_idx2 != -1)
        	{
        		printf("2. Three of a Kind\n");
        	}
            scanf("%d", &combo);
        }

        if (combo == 2)
        {
            play_card(game, player_id, special_idx1);
            play_card(game, player_id, special_idx2);
            draw_card_from_player(game, player_id, false);
        } else if (combo == 1) {
            play_card(game, player_id, special_idx1);
            draw_card_from_player(game, player_id, true);
        }
    }
}

void handle_favor(Game* game, int player_id)
{
    int next_player_idx = (player_id == 0) ? 1 : 0;
    int card_index;

    printf("Player %d's hand:\n", next_player_idx + 1);
    print_cards(game->players[next_player_idx].hand, game->players[next_player_idx].num_cards);
    printf("Card index to draw(0-%d): ", game->players[next_player_idx].num_cards - 1);
    scanf("%d", &card_index);

    game->players[player_id].hand[game->players[player_id].num_cards] = game->players[next_player_idx].hand[card_index];

    for (int i = card_index; i < game->players[next_player_idx].num_cards - 1; i++)
    {
        game->players[next_player_idx].hand[i] = game->players[next_player_idx].hand[i + 1];
    }

    game->players[player_id].num_cards++;
    game->players[next_player_idx].num_cards--;
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
    int shuffle_on, nope_on;
    bool nope = false;
    int next_player_idx = (player_id == 0) ? 1 : 0;

    game->pile[(game->pile_index)++] = game->players[player_id].hand[card_index];

    for (int i = card_index; i < game->players[player_id].num_cards - 1; i++) {
        game->players[player_id].hand[i] = game->players[player_id].hand[i + 1];
    }
    game->players[player_id].num_cards--;

    do {
      	nope_on = 0;
    	int nope_index = search_card(game->players[next_player_idx].hand, game->players[next_player_idx].num_cards, NOPE, -1);
    	if (nope_index != -1) {
      		printf("Player %d, do you want to play NOPE card? (0 or 1) ", next_player_idx + 1);
        	scanf("%d", &nope_on);
        	if (nope_on) {
                nope = !nope;
                if (nope)
                {
                    printf("NOPE!\n");
                } else {
                    printf("YUP!\n");
                }
          		for (int i = nope_index; i < game->players[next_player_idx].num_cards - 1; i++)
    			{
        			game->players[next_player_idx].hand[i] = game->players[next_player_idx].hand[i + 1];
    			}

            	game->players[next_player_idx].num_cards--;
        	}
    	}
        next_player_idx = (next_player_idx == 0) ? 1 : 0;
    } while (nope_on);
	printf("Card played: ");
    print_cards(&game->pile[game->pile_index - 1], 1);
    if (nope) {
        return;
    }

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
            game->current_player_index = (player_id == 0) ? 1 : 0;
        	break;
        case SKIP:
        	if (game->attack_stack > 0) {
            	game->attack_stack--;
        	}
            game->skip_active = true;
            game->current_player_index = (player_id == 0) ? 1 : 0;
        	break;
        case SHUFFLE:
            do {
                shuffle_deck(game->deck, game->deck_index + 1);
                printf("Shuffle? (0 or 1) ");
                scanf("%d", &shuffle_on);
            } while (shuffle_on);

        	break;
        case TACOCAT:
            handle_special_card(game, player_id, TACOCAT);
            break;
        case CATTERMELON:
            handle_special_card(game, player_id, CATTERMELON);
            break;
        case POTATO:
            handle_special_card(game, player_id, POTATO);
            break;
        case BEARD:
            handle_special_card(game, player_id, BEARD);
            break;
        case RAINBOW:
            handle_special_card(game, player_id, RAINBOW);
            break;
        case FAVOR:
            handle_favor(game, player_id);
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
        case END_TURN:
          	draw_card(game, player_id);

            if (game->attack_stack > 0) {
                game->attack_stack--;
                if (game->attack_stack == 0) {
                    game->current_player_index = (player_id == 0) ? 1 : 0;
                }
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
    game->not_entered_special = true;
    Action action;

    do {
        printf("\nPlayer %d's turn\n", player_id + 1);
        printf("Hand:\n");
        print_cards(game->players[player_id].hand, game->players[player_id].num_cards);
        printf("Action:\n");
        printf("1. Play card\n");
        printf("2. End turn\n");
        printf("Choose action: ");
        scanf("%d", (int*)&action.type);
        handle_player_action(game, player_id, action);

        if (game->num_players == 1) {
            game->game_over = true;
            return;
        }

        if (game->skip_active && (game->attack_stack == 0 || game->safe_from_attack)) {
            game->safe_from_attack = false;
            game->skip_active = false;
            break;
        } else if (game->skip_active) {
            game->skip_active = false;
            action.type = END_TURN;
        }
    } while (game->attack_stack > 0 || action.type == PLAY_CARD);
}


