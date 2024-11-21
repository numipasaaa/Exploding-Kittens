#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#define MAX_PLAYERS 2
#define MAX_GAMES 10
#define MAX_CARDS 56

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
void print_cards(Card *hand, int n);
void handle_turn(Game* game, int player_id);

#endif //GAME_LOGIC_H
