#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>

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
    Player players[4];  // Max players per game
    Card deck[52];       // Deck of cards
    int deck_index;
    int current_player;   // Index of the current player
    int num_players;    // Current number of players in the game
    int num_active;     // Active players
    bool game_over;
} Game;

typedef struct {
    Game games[10];     // Array of games the server can manage
    int num_games;      // Number of active games
} ServerState;

void init_game(Game *game);
void init_deck(Card *deck);
void handle_player_action(Game *game, int player_id, char *action);

#endif //GAME_LOGIC_H
