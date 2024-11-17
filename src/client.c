#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ncurses.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

void connect_to_server(int *socket_fd);
void play_game(int socket_fd);

int main() {
    int socket_fd;
    connect_to_server(&socket_fd);

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    play_game(socket_fd);

    endwin(); // End ncurses mode
    close(socket_fd);
    return 0;
}

void connect_to_server(int *socket_fd) {
    struct sockaddr_in server_addr;
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(*socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        exit(1);
    }
    printf("Connected to server.\n");
}

void play_game(int socket_fd) {
    char buffer[BUFFER_SIZE];
    while (1) {
        // Display game state (mockup)
        printw("Waiting for server response...\n");
        refresh();

        // Receive message
        if (recv(socket_fd, buffer, sizeof(buffer), 0) > 0) {
            printw("Server: %s\n", buffer);
            refresh();
        }

        // Send a simple command (e.g., drawing a card)
        snprintf(buffer, BUFFER_SIZE, "DRAW_CARD");
        send(socket_fd, buffer, strlen(buffer), 0);
    }
}
