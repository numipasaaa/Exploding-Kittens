#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ncurses.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

void error(char *msg);

int main() {
    int socket_fd, n;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int card_num = 8;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        error("Error at opening socket.");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error("Connect failed.");
    }
    printf("Connected to server.\n");

    while (1) 
    {
        //printf("Waiting for game state...\n");

        bzero(buffer, BUFFER_SIZE - 1);
        n = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
        if (n < 0) {
            error("Error on reading");
        }
        printf("%s\n", buffer); // Afișăm direct starea jocului și mâna

        if (strncmp(buffer, "Your turn", 9) == 0)
        {
            printf("Enter action (DRAW_CARD/PLAY_CARD <index>): ");
            bzero(buffer, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE - 1, stdin);
            n = send(socket_fd, buffer, strlen(buffer), 0);
            if(n < 0)
            {
                error("Error on writing.");
            }
        }
    }

    /*Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    endwin(); // End ncurses mode*/

    close(socket_fd);
    return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}
