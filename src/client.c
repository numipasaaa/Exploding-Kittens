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
void play_game(int socket_fd, char *username);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Enter username!\n");
        exit(1);
    }
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);


    int socket_fd;
    connect_to_server(&socket_fd);

    play_game(socket_fd, argv[1]);

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

void play_game(int socket_fd, char *username) {
    int n;
    int c;
    char buffer[BUFFER_SIZE];

    send(socket_fd, username, BUFFER_SIZE, 0);

    while (1) {
        // Display game state (mockup)
        explicit_bzero(buffer, BUFFER_SIZE);
        n = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (n < 0) {
            perror("Error reading from socket");
        }
        buffer[n] = '\0';
        printw("%s", buffer);
        refresh();

        if (strstr(buffer, "Choose option: ") != NULL || strstr(buffer, "Play card:") != NULL) {
            explicit_bzero(buffer, BUFFER_SIZE);
            int i = 0;
            while ((c = getch()) != '\n' && c != EOF)
            {
                if (c != -1)
                {
                    buffer[i++] = c;
                    printw("%c", c);
                    refresh();
                }
            }
            buffer[i++] = '\n';
            printw("\n");

            buffer[i] = '\0';

            send(socket_fd, buffer, sizeof(buffer), 0);
        }
        if (strstr(buffer, "Choose option: ") != NULL || strstr(buffer, "to finish their turn...")) {
            clear();
        }
    }
}
