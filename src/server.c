#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include "game_logic.h"

#define PORT 12345

pthread_mutex_t game_lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void start_game(Game *game);
void send_message(int client_socket, const char *message);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    pthread_mutex_init(&game_lock, NULL);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_PLAYERS);

    printf("Server started on port %d...\n", PORT);

    Game *game = malloc(sizeof(Game));
    init_game(game);
    init_players_hands(game);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len))) {
        game->players[game->num_players].socket = client_socket;
        game->num_players++;
        pthread_create(&thread_id, NULL, handle_client, (void *)game);
        pthread_detach(thread_id);
    }

    while (1);

    close(server_socket);
    return 0;
}

void *handle_client(void *arg) {
    Game *game = (Game *)arg;
    int player_socket = game->players[game->num_players - 1].socket;
    // char prev[BUFFER_SIZE] = "";
    char buffer[BUFFER_SIZE];

    explicit_bzero(buffer, BUFFER_SIZE);
    read(player_socket, buffer, BUFFER_SIZE - 1);
    buffer[strlen(buffer)] = '\0';
    strcpy(game->players[game->num_players - 1].name, buffer);

    printf("%s connected.\n", game->players[game->num_players - 1].name);


    while (!game->game_over)
    {
        if (game->num_players < MAX_PLAYERS)
        {
            explicit_bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "Waiting for other players to join...\n");
            write(player_socket, buffer, sizeof(buffer));
            // strcpy(prev, "Waiting for other players to join...\n");
            while (game->num_players < MAX_PLAYERS);
        }
        else
        {
            pthread_mutex_lock(&game_lock);
            handle_turn(game, player_socket);
            pthread_mutex_unlock(&game_lock);
        }
    }

    close(player_socket);
    return NULL;
}

void send_message(int client_socket, const char *message) {
    send(client_socket, message, BUFFER_SIZE, 0);
}

