#include "game_logic.h"
#include <sys/stat.h>

void print_cards(int socket, Card* hand, int n)
{
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    explicit_bzero(buffer, BUFFER_SIZE);
    explicit_bzero(temp, BUFFER_SIZE);

	for (int i = 0; i < n; i++)
    {
    	if (n > 1)
        {
          	sprintf(temp, "Card %d: ", i + 1);
            strcat(buffer, temp);
   		}

        switch (hand[i].type)
        {
            case EXPLODING_KITTEN:
                strcat(buffer, "EXPLODING_KITTEN\n");
                break;
            case DEFUSE:
                strcat(buffer, "DEFUSE\n");
                break;
            case ATTACK:
                strcat(buffer, "ATTACK\n");
                break;
            case SKIP:
                strcat(buffer, "SKIP\n");
                break;
            case FAVOR:
                strcat(buffer, "FAVOR\n");
                break;
            case SHUFFLE:
                strcat(buffer, "SHUFFLE\n");
                break;
            case SEE_THE_FUTURE:
                strcat(buffer, "SEE_THE_FUTURE\n");
                break;
            case NOPE:
                strcat(buffer, "NOPE\n");
                break;
            case TACOCAT:
                strcat(buffer, "TACOCAT\n");
                break;
            case CATTERMELON:
                strcat(buffer, "CATTERMELON\n");
                break;
            case POTATO:
                strcat(buffer, "POTATO\n");
                break;
            case BEARD:
                strcat(buffer, "BEARD\n");
                break;
            case RAINBOW:
                strcat(buffer, "RAINBOW\n");
                break;
        }
    }
    write(socket, buffer, sizeof(buffer));
}

void print_cards_server(Card* hand, int n)
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
    for (int i = 0; i < MAX_PLAYERS; i++) {
        for (int j = 0; j < 7; j++) {
            game->players[i].hand[j] = game->deck[game->deck_index];
            game->deck_index--;
        }
        game->players[i].hand[7].type = DEFUSE;
        game->players[i].num_cards = 8;
        shuffle_deck(game->players[i].hand, 8);
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
    game->rematch[0] = 2;
    game->rematch[1] = 2;
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
    char buffer[3];
    char buf[BUFFER_SIZE];

    explicit_bzero(buf, BUFFER_SIZE);
    sprintf(buf, "Position to reinsert(1-%d)\n", game->deck_index + 1);
    write(game->players[player_id].socket, buf, sizeof(buf));

    explicit_bzero(buf, BUFFER_SIZE);
    strcpy(buf, "Choose option: ");
    write(game->players[player_id].socket, buf, sizeof(buf));

    explicit_bzero(buffer, 3);
    while (read(game->players[player_id].socket, buffer, 2))
    {
        buffer[2] = '\0';

        if (buffer[1] == '\n')
        {
            if ('1' <= buffer[0] && buffer[0] <= '9')
            {
                index = strtol(buffer, NULL, 10);
                break;
            }
        } else if (('1' <= buffer[0] && buffer[0] <= '9') && ('0' <= buffer[1] && buffer[1] <= '9'))
        {
            index = strtol(buffer, NULL, 10);
            break;
        }
        explicit_bzero(buffer, 3);
    }
    if (index > game->deck_index + 1)
    {
        index = game->deck_index + 1;
    }
    if (index < 1)
    {
        index = 1;
    }
    index--;

    for (int i = game->deck_index; i > index; i--)
    {
        game->deck[i] = game->deck[i - 1];
    }

    game->deck[index].type = EXPLODING_KITTEN;
}

void handle_explosion(Game* game, int player_id)
{
    int next_player_idx = next_player_idx = (player_id == 0) ? 1 : 0;
    char buf[BUFFER_SIZE];
    int defuse_index = search_card(game->players[player_id].hand, game->players[player_id].num_cards, DEFUSE, -1);
    if (defuse_index != -1)
    {
        use_defuse(game, player_id, defuse_index);
        reinsert_exploding_kitten(game, player_id);

    }
    else
    {
        explicit_bzero(buf, BUFFER_SIZE);
        strcpy(buf, "You exploded! :3\n");
        write(game->players[player_id].socket, buf, sizeof(buf));

        explicit_bzero(buf, BUFFER_SIZE);
        strcpy(buf, "You won! :D\n");
        write(game->players[next_player_idx].socket, buf, sizeof(buf));
        printf("(Game %d) %s won!\n", game->game_idx + 1, game->players[next_player_idx].name);
        game->num_players--;
    }
}

void draw_card_from_player(Game* game, int player_id, bool random)
{
	int next_player_idx = (player_id == 0) ? 1 : 0;
    int card_index, card;
    char buf[BUFFER_SIZE];
    char buffer[3];

    if (random) {
        explicit_bzero(buf, BUFFER_SIZE);
        sprintf(buf, "Card position to draw(1-%d)\n", game->players[next_player_idx].num_cards);
        write(game->players[player_id].socket, buf, sizeof(buf));

        explicit_bzero(buf, BUFFER_SIZE);
        strcpy(buf, "Choose option: ");
        write(game->players[player_id].socket, buf, sizeof(buf));

        explicit_bzero(buffer, 3);
        while (read(game->players[player_id].socket, buffer, 2))
        {
            buffer[2] = '\0';

            if (buffer[1] == '\n')
            {
                if ('1' <= buffer[0] && buffer[0] <= '9')
                {
                    card_index = strtol(buffer, NULL, 10);
                    break;
                }
            } else if (('1' <= buffer[0] && buffer[0] <= '9') && ('0' <= buffer[1] && buffer[1] <= '9'))
            {
                card_index = strtol(buffer, NULL, 10);
                break;
            }
            explicit_bzero(buffer, 3);
        }
        if (card_index > game->players[next_player_idx].num_cards)
        {
            card_index = game->players[next_player_idx].num_cards;
        }
        if (card_index < 1)
        {
            card_index = 1;
        }
        card_index--;
		game->players[player_id].hand[game->players[player_id].num_cards] = game->players[next_player_idx].hand[card_index];
    } else {
        explicit_bzero(buf, BUFFER_SIZE);
        strcpy(buf, "Card to draw:\n");
        write(game->players[player_id].socket, buf, sizeof(buf));

        explicit_bzero(buf, BUFFER_SIZE);
        strcpy(buf, "Choose option: ");
        write(game->players[player_id].socket, buf, sizeof(buf));

        explicit_bzero(buffer, 3);
        while (read(game->players[player_id].socket, buffer, 2))
        {
            buffer[2] = '\0';

            if (buffer[1] == '\n')
            {
                if ('1' <= buffer[0] && buffer[0] <= '9')
                {
                    card = strtol(buffer, NULL, 10);
                    break;
                }
            } else if (('1' == buffer[0]) && ('0' <= buffer[1] && buffer[1] <= '2'))
            {
                card = strtol(buffer, NULL, 10);
                break;
            }
            explicit_bzero(buffer, 3);
        }

        card_index = search_card(game->players[next_player_idx].hand, game->players[next_player_idx].num_cards, card, -1);

        if (card_index == -1)
        {
            strcpy(buf, "Card not found.\n");
            write(game->players[player_id].socket, buf, sizeof(buf));
            return;
        }

        game->players[player_id].hand[game->players[player_id].num_cards] = game->players[next_player_idx].hand[card_index];
    }

    for (int i = card_index; i < game->players[next_player_idx].num_cards - 1; i++)
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
    char buffer[2];
    char buf[BUFFER_SIZE];

    if (game->players[player_id].not_entered_special)
    {
      	if (special_idx1 != -1)
        {
            explicit_bzero(buf, BUFFER_SIZE);
        	strcpy(buf, "Actions:\n1. Two of a Kind\n");
            if (special_idx2 != -1)
        	{
        		strcat(buf, "2. Three of a Kind\n");
        	}
            write(game->players[player_id].socket, buf, sizeof(buf));

            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "Choose option: ");
            write(game->players[player_id].socket, buf, sizeof(buf));

            explicit_bzero(buffer, 2);
            while (read(game->players[player_id].socket, buffer, 1))
            {
                buffer[1] = '\0';

                if (buffer[0] == '1' || buffer[0] == '2')
                {
                    combo = strtol(buffer, NULL, 10);
                    break;
                }
                explicit_bzero(buffer, 2);
            }
        }

        if (combo == 2)
        {
            printf("(Game %d) Three of a Kind!\n", game->game_idx + 1);
            game->players[player_id].not_entered_special = false;
            play_card(game, player_id, special_idx1);
            play_card(game, player_id, special_idx2);
            draw_card_from_player(game, player_id, false);
        } else if (combo == 1) {
            printf("(Game %d) Two of a Kind!\n", game->game_idx + 1);
            game->players[player_id].not_entered_special = false;
            play_card(game, player_id, special_idx1);
            draw_card_from_player(game, player_id, true);
        }
    }
    game->players[player_id].not_entered_special = true;
}

void handle_favor(Game* game, int player_id)
{
    int next_player_idx = (player_id == 0) ? 1 : 0;
    int card_index;
    char buf[BUFFER_SIZE];
    char buffer[3];

    explicit_bzero(buf, BUFFER_SIZE);
    sprintf(buf, "%s's hand:\n", game->players[next_player_idx].name);
    write(game->players[next_player_idx].socket, buf, sizeof(buf));

    print_cards(game->players[next_player_idx].socket, game->players[next_player_idx].hand, game->players[next_player_idx].num_cards);

    explicit_bzero(buf, BUFFER_SIZE);
    sprintf(buf, "Card position to draw(1-%d)\n", game->players[next_player_idx].num_cards);
    write(game->players[next_player_idx].socket, buf, sizeof(buf));

    explicit_bzero(buf, BUFFER_SIZE);
    strcpy(buf, "Choose option: ");
    write(game->players[next_player_idx].socket, buf, sizeof(buf));

    explicit_bzero(buffer, 3);
    while (read(game->players[next_player_idx].socket, buffer, 2))
    {
        buffer[2] = '\0';
        if (buffer[1] == '\n')
        {
            if ('1' <= buffer[0] && buffer[0] <= '9')
            {
                card_index = strtol(buffer, NULL, 10);
                break;
            }
        } else if (('1' <= buffer[0] && buffer[0] <= '9') && ('0' <= buffer[1] && buffer[1] <= '9'))
        {
            card_index = strtol(buffer, NULL, 10);
            break;
        }
        explicit_bzero(buffer, 3);
    }

    if (card_index > game->players[next_player_idx].num_cards)
    {
        card_index = game->players[next_player_idx].num_cards;
    }
    if (card_index < 1)
    {
        card_index = 1;
    }

    card_index--;
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
        printf("(Game %d) EXPLODING KITTEN!\n", game->game_idx + 1);

        char buf[BUFFER_SIZE];
        explicit_bzero(buf, BUFFER_SIZE);
        strcpy(buf, "EXPLODING KITTEN!\n");
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            write(game->players[i].socket, buf, sizeof(buf));
        }

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
    char buf[BUFFER_SIZE];
    char buffer[2];
    bool nope = false;
    int next_player_idx = (player_id == 0) ? 1 : 0;

    game->pile[(game->pile_index)++] = game->players[player_id].hand[card_index];

    for (int i = card_index; i < game->players[player_id].num_cards - 1; i++) {
        game->players[player_id].hand[i] = game->players[player_id].hand[i + 1];
    }
    game->players[player_id].num_cards--;

    if (game->players[player_id].not_entered_special) {
        do {
            nope_on = 0;
            int nope_index = search_card(game->players[next_player_idx].hand, game->players[next_player_idx].num_cards, NOPE, -1);
            if (nope_index != -1) {
                explicit_bzero(buf, BUFFER_SIZE);
                sprintf(buf, "%s, do you want to play NOPE card?\n", game->players[next_player_idx].name);
                write(game->players[next_player_idx].socket, buf, sizeof(buf));

                explicit_bzero(buf, BUFFER_SIZE);
                strcpy(buf, "Choose option: ");
                write(game->players[next_player_idx].socket, buf, sizeof(buf));

                explicit_bzero(buffer, 2);
                while (read(game->players[next_player_idx].socket, buffer, 1))
                {
                    buffer[1] = '\0';

                    if (buffer[0] == '1' || buffer[0] == '2')
                    {
                        nope_on = strtol(buffer, NULL, 10) - 1;
                        break;
                    }
                    explicit_bzero(buffer, 2);
                }
                if (nope_on == 1) {
                    char buf2[BUFFER_SIZE];
                    explicit_bzero(buf2, BUFFER_SIZE);
                    nope = !nope;
                    if (nope)
                    {
                        printf("(Game %d) NOPE!\n", game->game_idx + 1);
                        strcpy(buf2, "NOPE!\n");
                        for (int i = 0; i < MAX_PLAYERS; i++)
                        {
                            write(game->players[i].socket, buf2, sizeof(buf2));
                        }
                    } else {
                        printf("(Game %d) YUP!\n", game->game_idx + 1);
                        strcpy(buf2, "YUP!\n");
                        for (int i = 0; i < MAX_PLAYERS; i++)
                        {
                            write(game->players[i].socket, buf2, sizeof(buf2));
                        }
                    }
                    for (int i = nope_index; i < game->players[next_player_idx].num_cards - 1; i++)
                    {
                        game->players[next_player_idx].hand[i] = game->players[next_player_idx].hand[i + 1];
                    }

                    game->players[next_player_idx].num_cards--;
                }
            }
            next_player_idx = (next_player_idx == 0) ? 1 : 0;
        } while (nope_on == 1);

        next_player_idx = (player_id == 0) ? 1 : 0;

        if (nope) {
            return;
        }
    }

    printf("(Game %d) Card played: ", game->game_idx + 1);
    print_cards_server(&game->pile[game->pile_index - 1], 1);

    switch (game->pile[game->pile_index - 1].type) {
    	case SEE_THE_FUTURE:
            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "Future cards:\n");
            write(game->players[player_id].socket, buf, sizeof(buf));
            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "1. "); write(game->players[player_id].socket, buf, sizeof(buf));
            print_cards(game->players[player_id].socket, &game->deck[game->deck_index], 1);

            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "2. "); write(game->players[player_id].socket, buf, sizeof(buf));
            print_cards(game->players[player_id].socket, &game->deck[game->deck_index - 1], 1);

            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "3. "); write(game->players[player_id].socket, buf, sizeof(buf));
            print_cards(game->players[player_id].socket, &game->deck[game->deck_index - 2], 1);
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
        	} else {
                game->skip_active = true;
                game->current_player_index = (player_id == 0) ? 1 : 0;
            }

        	break;
        case SHUFFLE:
            do {
                shuffle_deck(game->deck, game->deck_index + 1);

                explicit_bzero(buf, BUFFER_SIZE);
                strcpy(buf, "Shuffle?\n");
                write(game->players[next_player_idx].socket, buf, sizeof(buf));

                explicit_bzero(buf, BUFFER_SIZE);
                strcpy(buf, "Choose option: ");
                write(game->players[next_player_idx].socket, buf, sizeof(buf));

                explicit_bzero(buffer, 2);
                while (read(game->players[next_player_idx].socket, buffer, 1))
                {
                    buffer[1] = '\0';

                    if (buffer[0] == '1' || buffer[0] == '2')
                    {
                        shuffle_on = strtol(buffer, NULL, 10) - 1;
                        break;
                    }
                    explicit_bzero(buffer, 2);
                }
            } while (shuffle_on == 1);

        	break;
        case TACOCAT:
            if (game->players[player_id].not_entered_special)
            {
                handle_special_card(game, player_id, TACOCAT);
            }
            break;
        case CATTERMELON:
            if (game->players[player_id].not_entered_special)
            {
                handle_special_card(game, player_id, CATTERMELON);
            }
            break;
        case POTATO:
            if (game->players[player_id].not_entered_special)
            {
                handle_special_card(game, player_id, POTATO);
            }
            break;
        case BEARD:
            if (game->players[player_id].not_entered_special)
            {
                handle_special_card(game, player_id, BEARD);
            }
            break;
        case RAINBOW:
            if (game->players[player_id].not_entered_special)
            {
                handle_special_card(game, player_id, RAINBOW);
            }
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
    char buffer[3];
    char buf[BUFFER_SIZE] = "";
    switch (action.type) {
        case PLAY_CARD:
            printf("(Game %d) %s: Play card\n", game->game_idx + 1, game->players[player_id].name);

            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "Play card: ");
            write(game->players[player_id].socket, buf, sizeof(buf));

            explicit_bzero(buffer, 3);
            while (read(game->players[player_id].socket, buffer, 2))
            {
                buffer[2] = '\0';

                if (buffer[1] == '\n')
                {
                    if ('1' <= buffer[0] && buffer[0] <= '9')
                    {
                        card_index = strtol(buffer, NULL, 10);
                        break;
                    }
                } else if (('1' <= buffer[0] && buffer[0] <= '9') && ('0' <= buffer[1] && buffer[1] <= '9'))
                {
                    card_index = strtol(buffer, NULL, 10);
                    break;
                }
                explicit_bzero(buffer, 3);
            }
            card_index--;
            play_card(game, player_id, card_index);
            break;
        case END_TURN:
            printf("(Game %d) %s: End turn\n", game->game_idx + 1, game->players[player_id].name);
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
            // printf("Invalid action\n");
            break;
    }
}

void handle_turn(Game* game, int socket)
{
    int player_id = game->current_player_index;

    Action action;
    int client_idx;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->players[i].socket == socket) {
            client_idx = i;
            break;
        }
    }
    game->players[client_idx].not_entered_special = true;
    do {
        char buf[BUFFER_SIZE];

        explicit_bzero(buf, BUFFER_SIZE);
        sprintf(buf, "\n%s's turn\n", game->players[player_id].name);
        strcat(buf, "Your hand:\n");
        write(socket, buf, sizeof(buf));

        print_cards(socket, game->players[client_idx].hand, game->players[client_idx].num_cards);

        if (client_idx == player_id) {
            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "Actions:\n");
            strcat(buf, "1. Play card\n");
            strcat(buf, "2. End turn\n");
            write(socket, buf, sizeof(buf));

            explicit_bzero(buf, BUFFER_SIZE);
            strcpy(buf, "Choose option: ");
            write(socket, buf, sizeof(buf));

            char buffer[2];
            explicit_bzero(buffer, 2);
            while (read(socket, buffer, 1))
            {
                buffer[1] = '\0';

                if (buffer[0] == '1' || buffer[0] == '2')
                {
                    action.type = strtol(buffer, NULL, 10);
                    break;
                }
                explicit_bzero(buffer, 2);
            }

            handle_player_action(game, player_id, action);
        }
        else {
            explicit_bzero(buf, BUFFER_SIZE);
            sprintf(buf, "Waiting for %s to finish their turn...\n", game->players[game->current_player_index].name);
            write(socket, buf, sizeof(buf));
            sleep(1);
            break;
        }

        if (game->num_players == 1) {
            game->game_over = true;
            printf("(Game %d) Game over!\n", game->game_idx + 1);
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                explicit_bzero(buf, BUFFER_SIZE);
                strcpy(buf, "Do you want a rematch?\nYou will have to wait 10 seconds for the game to restart.\n");
                write(game->players[i].socket, buf, sizeof(buf));

                explicit_bzero(buf, BUFFER_SIZE);
                strcpy(buf, "Choose option: ");
                write(game->players[i].socket, buf, sizeof(buf));

                char buffer[2];
                explicit_bzero(buffer, 2);
                while (read(game->players[i].socket, buffer, 1))
                {
                    buffer[1] = '\0';

                    if (buffer[0] == '1' || buffer[0] == '2')
                    {
                        game->rematch[i] = strtol(buffer, NULL, 10) - 1;
                        break;
                    }
                    explicit_bzero(buffer, 2);
                }
            }

            while (game->rematch[0] == 2 || game->rematch[1] == 2)
            {
                sleep(1);
            }

            if (game->rematch[0] == 1 && game->rematch[1] == 1)
            {
                printf("(Game %d) Rematch!\n", game->game_idx + 1);
                init_game(game);
                game->num_players = MAX_PLAYERS;
                init_players_hands(game);
            }

            break;
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


