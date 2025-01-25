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

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
// Implementation of citoa()
char* citoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

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
    int card_sz = 0;
    int action_sz = 0;
    char actions[BUFFER_SIZE];
    char action_lines[2][BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char cards[BUFFER_SIZE];
    char card_lines[BUFFER_SIZE][BUFFER_SIZE];
    char prev_buffer[BUFFER_SIZE] = "";
    char booleans[2][10] = {"false", "true"};
    char card_collection[12][BUFFER_SIZE] = {
        "DEFUSE",
        "ATTACK",
        "SKIP",
        "FAVOR",
        "SHUFFLE",
        "SEE_THE_FUTURE",
        "NOPE",
        "TACOCAT",
        "CATTERMELON",
        "POTATO",
        "BEARD",
        "RAINBOW"
    };

    send(socket_fd, username, BUFFER_SIZE, 0);

    while (1) {
        // Display game state (mockup)
        explicit_bzero(buffer, BUFFER_SIZE);
        n = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (n < 0) {
            continue;
        }
        buffer[n] = '\0';
        if ((strstr(buffer, "Actions:") == NULL) && (strstr(buffer, "Choose option:") == NULL) && (strstr(buffer, "Play card:") == NULL)) {
            printw("%s", buffer);
        }
        refresh();

        if (strstr(buffer, "Play card:") != NULL) {
            int yMax, xMax;
            getmaxyx(stdscr, yMax, xMax);

            WINDOW *menuwin = newwin(yMax - 2, xMax - 2, 1, 1);
            box(menuwin, 0, 0);
            refresh();
            wrefresh(menuwin);

            keypad(menuwin, TRUE);

            int choice, highlight = 0;
            while (1) {
                for (int i = 0; i < card_sz; i++) {
                    if (i == highlight) {
                        wattron(menuwin, A_REVERSE);
                    }
                    mvwprintw(menuwin, i + 1, 1, card_lines[i]);
                    wattroff(menuwin, A_REVERSE);
                }
                choice = wgetch(menuwin);

                switch (choice) {
                    case KEY_UP:
                        highlight--;
                        if (highlight == -1) {
                            highlight = 0;
                        }
                        break;
                    case KEY_DOWN:
                        highlight++;
                        if (highlight == card_sz) {
                            highlight = card_sz - 1;
                        }
                        break;
                    default:
                        break;
                }
                if (choice == 10) {
                    break;
                }
            }
            citoa(highlight + 1, buffer, 10);
            buffer[strlen(buffer)] = '\n';
            buffer[strlen(buffer) + 1] = '\0';

            send(socket_fd, buffer, sizeof(buffer), 0);
            clear();
        }

        if (strstr(buffer, "Choose option: ") != NULL) {
            if (strstr(prev_buffer, "Actions:") != NULL)
            {
                int yMax, xMax, y, x;
                WINDOW *menuwin;
                getmaxyx(stdscr, yMax, xMax);
                getyx(stdscr, y, x);

                if ((yMax - 4) < y + 1) {
                    menuwin = newwin(4, xMax - 2, yMax - 4, 1);
                } else {
                    menuwin = newwin(4, xMax - 2, y + 1, 1);
                }

                box(menuwin, 0, 0);
                refresh();
                wrefresh(menuwin);

                keypad(menuwin, TRUE);

                int choice, highlight = 0;
                while (1) {
                    for (int i = 0; i < action_sz; i++) {
                        if (i == highlight) {
                            wattron(menuwin, A_REVERSE);
                        }
                        mvwprintw(menuwin, i + 1, 1, action_lines[i]);
                        wattroff(menuwin, A_REVERSE);
                    }
                    choice = wgetch(menuwin);

                    switch (choice) {
                        case KEY_UP:
                            highlight--;
                            if (highlight == -1) {
                                highlight = 0;
                            }
                            break;
                        case KEY_DOWN:
                            highlight++;
                            if (highlight == action_sz) {
                                highlight = action_sz - 1;
                            }
                            break;
                        default:
                            break;
                    }
                    if (choice == 10) {
                        break;
                    }
                }
                citoa(highlight + 1, buffer, 10);
                buffer[strlen(buffer)] = '\n';
                buffer[strlen(buffer) + 1] = '\0';

                send(socket_fd, buffer, sizeof(buffer), 0);
                clear();

            } else if (strstr(prev_buffer, "?") != NULL)
            {
                int yMax, xMax, y, x;
                WINDOW *menuwin;
                getmaxyx(stdscr, yMax, xMax);
                getyx(stdscr, y, x);

                if ((yMax - 4) < y + 1) {
                    menuwin = newwin(4, xMax - 2, yMax - 4, 1);
                } else {
                    menuwin = newwin(4, xMax - 2, y + 1, 1);
                }

                box(menuwin, 0, 0);
                refresh();
                wrefresh(menuwin);

                keypad(menuwin, TRUE);

                int choice, highlight = 0;
                while (1) {
                    for (int i = 0; i < 2; i++) {
                        if (i == highlight) {
                            wattron(menuwin, A_REVERSE);
                        }
                        mvwprintw(menuwin, i + 1, 1, booleans[i]);
                        wattroff(menuwin, A_REVERSE);
                    }
                    choice = wgetch(menuwin);

                    switch (choice) {
                        case KEY_UP:
                            highlight--;
                            if (highlight == -1) {
                                highlight = 0;
                            }
                            break;
                        case KEY_DOWN:
                            highlight++;
                            if (highlight == 2) {
                                highlight = 1;
                            }
                            break;
                        default:
                            break;
                    }
                    if (choice == 10) {
                        break;
                    }
                }
                citoa(highlight + 1, buffer, 10);
                buffer[strlen(buffer)] = '\n';
                buffer[strlen(buffer) + 1] = '\0';

                send(socket_fd, buffer, sizeof(buffer), 0);
                clear();
            } else if (strstr(prev_buffer, "Card to draw:") != NULL) {
                int yMax, xMax;
                getmaxyx(stdscr, yMax, xMax);

                WINDOW *menuwin = newwin(yMax - 2, xMax - 2, 1, 1);
                box(menuwin, 0, 0);
                refresh();
                wrefresh(menuwin);

                keypad(menuwin, TRUE);

                int choice, highlight = 0;
                while (1) {
                    for (int i = 0; i < 12; i++) {
                        if (i == highlight) {
                            wattron(menuwin, A_REVERSE);
                        }
                        mvwprintw(menuwin, i + 1, 1, card_collection[i]);
                        wattroff(menuwin, A_REVERSE);
                    }
                    choice = wgetch(menuwin);

                    switch (choice) {
                        case KEY_UP:
                            highlight--;
                            if (highlight == -1) {
                                highlight = 0;
                            }
                            break;
                        case KEY_DOWN:
                            highlight++;
                            if (highlight == 12) {
                                highlight = 11;
                            }
                            break;
                        default:
                            break;
                    }
                    if (choice == 10) {
                        break;
                    }
                }
                citoa(highlight + 1, buffer, 10);
                buffer[strlen(buffer)] = '\n';
                buffer[strlen(buffer) + 1] = '\0';

                send(socket_fd, buffer, sizeof(buffer), 0);
                clear();
            } else {
                explicit_bzero(buffer, BUFFER_SIZE);
                int yMax, xMax, y, x;
                WINDOW *menuwin;
                getmaxyx(stdscr, yMax, xMax);
                getyx(stdscr, y, x);

                if ((yMax - 4) < y + 1) {
                    menuwin = newwin(4, xMax - 2, yMax - 4, 1);
                } else {
                    menuwin = newwin(4, xMax - 2, y + 1, 1);
                }

                box(menuwin, 0, 0);
                refresh();
                wrefresh(menuwin);
                int i = 0;
                while ((c = wgetch(menuwin)) != '\n' && c != EOF)
                {
                    if (c != -1)
                    {
                        getyx(menuwin, y, x);
                        buffer[i++] = c;
                        if (i == 1)
                            mvwprintw(menuwin, y + 1, x + 1, "%c", c);
                        else
                            wprintw(menuwin, "%c", c);
                        refresh();
                    }
                }
                buffer[i++] = '\n';

                buffer[i] = '\0';
                clear();
                send(socket_fd, buffer, sizeof(buffer), 0);
            }
        }
        if (strstr(prev_buffer, "Your hand:") != NULL) {
            strcpy(cards, buffer);
            char *p = NULL;
            p = strtok(cards, "\n");
            card_sz = 0;
            while (p != NULL) {
                strcpy(card_lines[card_sz], p);
                p = strtok(NULL, "\n");
                card_sz++;
            }
        }

        if (strstr(buffer, "Actions:") != NULL) {
            strcpy(actions, buffer);
            char *p = NULL;
            p = strtok(actions, "\n");
            p = strtok(NULL, "\n");
            action_sz = 0;
            while (p != NULL) {
                strcpy(action_lines[action_sz], p);
                p = strtok(NULL, "\n");
                action_sz++;
            }
        }

        if (strstr(buffer, "Choose option: ") != NULL || strstr(buffer, "to finish their turn...")) {
            clear();
        }
        strcpy(prev_buffer, buffer);
    }
}
