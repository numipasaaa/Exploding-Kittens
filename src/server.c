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

int main(void) {
    int server_socket;
    struct sockaddr_in server_addr;
    pthread_t th[MAX_GAMES];
    pthread_mutex_init(&game_lock, NULL);
    Game games[MAX_GAMES];
    ServerData server_data[MAX_GAMES];

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind");
        exit(1);
    }

    printf("Server started on port %d...\n", PORT);
    for (int i = 0; i < MAX_GAMES; i++)
    {
        init_game(&games[i]);
        init_players_hands(&games[i]);
        games[i].game_idx = i;
        server_data[i].game = &games[i];
        server_data[i].server_socket = server_socket;

        if (pthread_create(&th[i], NULL, handle_game, (void *)&server_data[i]) != 0)
        {
            exit(EXIT_FAILURE);
        }
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
    int r = 0;

    if (listen(server_socket, MAX_PLAYERS) < 0) {
        perror("Listen");
        exit(1);
    }

    if ((r = pthread_mutex_lock(&game_lock)) != 0)
    {
        fprintf(stderr, "Mutex lock error: %s\n", strerror(r));
        pthread_exit(NULL);
    }
    while ((game->num_players < MAX_PLAYERS) && (client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len))) {

        game->players[count++].socket = client_socket;
        game->num_players = count;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)game) != 0)
        {
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread_id);
    }
    if ((r = pthread_mutex_unlock(&game_lock)) != 0)
    {
        fprintf(stderr, "Mutex unlock error: %s\n", strerror(r));
        pthread_exit(NULL);
    }

    return NULL;
}

void *handle_client(void *arg) {
    Game *game = (Game *)arg;
    int player_socket = game->players[game->num_players - 1].socket;
    int player_id = game->num_players - 1;
    char buffer[BUFFER_SIZE];

    explicit_bzero(buffer, BUFFER_SIZE);
    read(player_socket, buffer, BUFFER_SIZE - 1);
    buffer[strlen(buffer)] = '\0';
    strcpy(game->players[player_id].name, buffer);

    printf("%s connected.\n", game->players[player_id].name);

    while (!game->game_over)
    {
        if (game->num_players < MAX_PLAYERS)
        {
            explicit_bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "Waiting for other players to join...\n");
            write(player_socket, buffer, sizeof(buffer));
            while (game->num_players < MAX_PLAYERS);
        }
        else
        {
            handle_turn(game, player_socket);
        }
        if (game->game_over)
        {
            sleep(10);
        }
    }

    printf("%s disconnected.\n", game->players[player_id].name);
    close(player_socket);
    return NULL;
}

