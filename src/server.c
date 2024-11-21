#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include "game_logic.h"

#define PORT 12345
#define BUFFER_SIZE 1024


void *handle_client(void *arg);
void start_game(Game *game);
void send_message(int client_socket, const char *message);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_PLAYERS);

    printf("Server started on port %d...\n", PORT);

    Game *game = malloc(sizeof(Game));
    init_game(game);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len))) {
        if (game->num_players < MAX_PLAYERS) {
            game->players[game->num_players].socket = client_socket;
            snprintf(game->players[game->num_players].name, 50, "Player%d", game->num_players + 1);
            game->num_players++;
            pthread_create(&thread_id, NULL, handle_client, (void *)game);
        }
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *arg) {
    Game *game = (Game *)arg;
    int player_socket = game->players[game->num_players - 1].socket;
    char buffer[BUFFER_SIZE];
    while (recv(player_socket, buffer, sizeof(buffer), 0) > 0) {
        printf("Received: %s\n", buffer);
        send_message(player_socket, "Message received.");
    }
    close(player_socket);
    return NULL;
}

void send_message(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
}

