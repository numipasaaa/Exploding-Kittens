#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define _XOPEN_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdbool.h>
#define MAX_PLAYERS 2
#define MAX_GAMES 2
#define MAX_CARDS 56
#define BUFFER_SIZE 1024

typedef enum
{
    EXPLODING_KITTEN,
    DEFUSE,
    ATTACK,
    SKIP,
    FAVOR,
    SHUFFLE,
    SEE_THE_FUTURE,
    NOPE,
    TACOCAT,
    CATTERMELON,
    POTATO,
    BEARD,
    RAINBOW
} CardType;

typedef struct
{
    CardType type;
} Card;

typedef enum
{
    DRAW_CARD,
    PLAY_CARD,
    END_TURN
} ActionType;

typedef struct
{
    ActionType type;
    Card card;
} Action;

typedef struct {
    int socket;         // Player's socket connection
    char name[50];      // Player name
    Card hand[50];       // Player's hand
    int num_cards;      // Number of cards in hand
    bool not_entered_special;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];  // Max players per game
    Card deck[MAX_CARDS];       // Deck of cards
    Card pile[MAX_CARDS];      // Discard pile
    int deck_index;
    int pile_index;
    int current_player_index;   // Index of the current player
    int num_players;    // Current number of players in the game
    bool game_over;
    int attack_stack;
    bool skip_active;
    bool safe_from_attack;
    int game_idx;
} Game;

typedef struct {
    Game games[MAX_GAMES];     // Array of games the server can manage
    int num_games;      // Number of active games
} ServerState;

void init_game(Game *game);
void init_deck(Card *deck);
void handle_player_action(Game *game, int player_id, Action action);
void init_players_hands(Game *game);
void shuffle_deck(Card *deck, int n);
void print_cards(int socket, Card *hand, int n);
void print_cards_server(Card *hand, int n);
void handle_turn(Game* game, int socket);
int search_card(Card* hand, int hand_size, CardType type, int ignore_index);
void handle_special_card(Game* game, int player_id, CardType card);
void play_card(Game* game, int player_id, int card_index);
void use_defuse(Game* game, int player_id, int card_index);
void reinsert_exploding_kitten(Game* game, int player_id);
void handle_explosion(Game* game, int player_id);
void draw_card_from_player(Game* game, int player_id, bool random);
void handle_special_card(Game* game, int player_id, CardType card);
void handle_favor(Game* game, int player_id);
void draw_card(Game* game, int player_id);

#endif //GAME_LOGIC_H
