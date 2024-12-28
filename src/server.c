#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "game_logic.h"

#define PORT 12345
#define BUFFER_SIZE 1024

void error(const char *msg);
void *handle_client(void *arg);
void start_game(Game *game);
void broadcast_game_state(Game *game);

typedef struct {
    int client_socket;
    int player_id;
    Game *game;
} ClientData;

Game game;

int main() {
    int server_socket, client_socket;

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    server_socket = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM => TCP
    if(server_socket < 0)
    {
        error("Error opening Socket.");
    }

    bzero((char *)&server_addr, sizeof(server_addr)); //clears any data

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error("Binding Failed.");
    }

    listen(server_socket, MAX_PLAYERS);

    printf("Server started on port %d...\n", PORT);

    init_game(&game);

    while(game.num_players < MAX_PLAYERS)
    {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if(client_socket < 0)
        {
            perror("Error on Accept.");
        }

        printf("Player %d connected.\n", game.num_players + 1);
        game.players[game.num_players].socket = client_socket;
        snprintf(game.players[game.num_players].name, 50, "Player%d", game.num_players + 1);
        game.num_players++;

        ClientData *client_data = malloc(sizeof(client_data));
        client_data->client_socket = client_socket;
        client_data->player_id = game.num_players - 1;
        client_data->game = &game;

        pthread_create(&thread_id, NULL, handle_client, client_data);
    }

    start_game(&game);

    while (1) 
    {
        for (int i = 0; i < game.num_players; i++) {
            if (game.num_players == 0 || game.game_over) {
                close(server_socket);
                exit(0);
            }
        }
    }

    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *handle_client(void *arg) {

    ClientData *client_data = (ClientData*)arg;

    int client_socket = client_data->client_socket;
    int player_id = client_data->player_id;
    Game *game = client_data->game;

    char buffer[BUFFER_SIZE];
    int n;

    while (1) {
        if (game->current_player_index == player_id) {
            send(client_socket, "Your turn. Enter command (DRAW_CARD/PLAY_CARD <index>): ", 62, 0);
        } else {
            send(client_socket, "Waiting for other player's turn...\n", 36, 0);
        }

        bzero(buffer, BUFFER_SIZE);
        n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (n < 0)
        {
            error("Error on reading");
        }

        if (strncmp(buffer, "DRAW_CARD", 9) == 0) {
            draw_card(game, player_id);
        } else if (strncmp(buffer, "PLAY_CARD", 9) == 0) {
            int card_index = atoi(buffer + 10);
            play_card(game, player_id, card_index);
        } else {
            send(client_socket, "Invalid command.\n", 17, 0);
            continue;
        }

        game->current_player_index = (game->current_player_index + 1) % game->num_players;
        broadcast_game_state(game);
    }

    close(client_socket);

    return NULL;
}

void broadcast_game_state(Game *game) {
    char state[BUFFER_SIZE];
    
    for (int i = 0; i < game->num_players; i++) {
        if(i == game->current_player_index)
        {
            snprintf(state, BUFFER_SIZE - 1, "Your turn\n");
            send(game->players[i].socket, state, strlen(state), 0);
        }
        else
        {
            snprintf(state, BUFFER_SIZE - 1, "His turn.\n");
            send(game->players[i].socket, state, strlen(state), 0);
        }

        bzero(state, BUFFER_SIZE);
        strcat(state, "Your hand:\n");
        for (int j = 0; j < game->players[i].num_cards; j++) {
            char card_info[50];
            snprintf(card_info, 50, "Card %d: %d\n", j, game->players[i].hand[j].type);
            strcat(state, card_info);
        }
        send(game->players[i].socket, state, strlen(state), 0);
    }
}

void start_game(Game *game) {

    printf("Starting the game...\n");

    init_players_hands(game);

    game->current_player_index = 1;

    broadcast_game_state(game);
}
