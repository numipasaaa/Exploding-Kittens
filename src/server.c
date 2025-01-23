#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include "game_logic.h"
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 12345

typedef struct ServerData {
    Game *game;
    int server_socket;
}ServerData;


pthread_mutex_t game_lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void *handle_game(void *arg);

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    pthread_t th[MAX_GAMES];
    pthread_mutex_init(&game_lock, NULL);
    Game games[MAX_GAMES];
    ServerData server_data[MAX_GAMES];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Server started on port %d...\n", PORT);
    for (int i = 0; i < MAX_GAMES; i++)
    {
        init_game(&games[i]);
        init_players_hands(&games[i]);
        games[i].game_idx = i;
        server_data[i].game = &games[i];
        server_data[i].server_socket = server_socket;
        pthread_create(&th[i], NULL, handle_game, (void *)&server_data[i]);
        pthread_detach(th[i]);
        sleep(1);
    }

    while(1);

    close(server_socket);
    return 0;
}

void *handle_game(void *arg) {
    ServerData *server_data = (ServerData*)arg;
    int server_socket = server_data->server_socket;
    Game *game = server_data->game;
    int client_socket;
    struct sockaddr_in client_addr;
    pthread_t thread_id;
    socklen_t client_len = sizeof(client_addr);
    int count = 0;

    listen(server_socket, MAX_PLAYERS);

    pthread_mutex_lock(&game_lock);
    while ((game->num_players < MAX_PLAYERS) && (client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len))) {

        game->players[count++].socket = client_socket;
        game->num_players = count;

        pthread_create(&thread_id, NULL, handle_client, (void *)game);
        pthread_detach(thread_id);
    }
    pthread_mutex_unlock(&game_lock);

    return NULL;
}

void *handle_client(void *arg) {
    Game *game = (Game *)arg;
    int player_socket = game->players[game->num_players - 1].socket;
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
            // printf("%d\n", game->num_players);
            explicit_bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "Waiting for other players to join...\n");
            write(player_socket, buffer, sizeof(buffer));
            while (game->num_players < MAX_PLAYERS);
        }
        else
        {
            handle_turn(game, player_socket);
        }
    }

    close(player_socket);
    return NULL;
}

