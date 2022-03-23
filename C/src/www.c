#include "www.h"
#include "assets.h"
#include<ctype.h>

const unsigned char HTTP_OK[13] = "HTTP/1.0 200\n";
const unsigned char HTTP_NOT_FOUND[13] = "HTTP/1.0 403\n";
const unsigned char CONTENT_TYPE_HTML[25] = "Content-Type: text/html\n\n";
const unsigned char CONTENT_TYPE_PLAIN[26] = "Content-Type: text/plain\n\n";
const unsigned char CONTENT_TYPE_JAVASCRIPT[31] = "Content-Type: text/javascript\n\n";

#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
WebServer* CreateWebServerWithPort(uint16_t port) {
    srand(time(NULL));

    WebServer *server = (WebServer*) malloc(sizeof(WebServer));
    if (server == NULL) {
        perror("In malloc");
        exit(EXIT_FAILURE);
    }

    int randIndex = rand() % (sizeof(wordlist)/12u);
    while(wordlist[randIndex] != 0x0A) {
        randIndex++;
    }
    randIndex++;
    int i;
    for(i = 0; i < WORD_SIZE; i++) {
        server->secret_word[i] = tolower(wordlist[randIndex+i]);
    }
    printf("The word: %s\n", server->secret_word);

    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(port);
    memset(server->address.sin_zero, '\0', sizeof server->address.sin_zero);

    if (bind(server->server_fd, (struct sockaddr *)&server->address, sizeof(server->address))<0) {
        perror("In bind");
        close(server->server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server->server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    return server;
}
#else
WebServer* CreateWebServerWithFD(int fd) {
    srand(time(NULL));

    WebServer *server = (WebServer*) malloc(sizeof(WebServer));
    if (server == NULL) {
        perror("In malloc");
        exit(EXIT_FAILURE);
    }

    int randIndex = rand() % (sizeof(wordlist)/12u);
    while(wordlist[randIndex] != 0x0A) {
        randIndex++;
    }
    randIndex++;
    int i;
    for(i = 0; i < WORD_SIZE; i++) {
        server->secret_word[i] = tolower(wordlist[randIndex+i]);
    }
    printf("The word: %s\n", server->secret_word);
    server->server_fd = fd;
    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(8080);

    return server;
}
#endif
void DestroyWebServer(WebServer* server) {
#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
    close(server->server_fd);
    server->server_fd = 0;
#endif
    free(server);
    server = NULL;
}

void RunWebServer(WebServer *server) {
#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
    int addrlen = sizeof(server->address);
#endif
    char buffer[30000] = {0};
    const size_t bufferlen = 30000;
    printf("Listening for connections\n");
    while(1) {
        int new_socket;
#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
        if ((new_socket = accept(server->server_fd, (struct sockaddr *)&server->address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
#else
        new_socket = server->server_fd;
#endif

        memset(buffer,0,bufferlen);
        ssize_t bytes_read = read(new_socket ,buffer,bufferlen);
        if (bytes_read < 0) {
            fprintf(stderr, "Error code %ld from read(%d): %s\n", bytes_read, new_socket, strerror(errno));
            return;
        }
        int i;
        printf("First 20 bytes: ");
        for(i = 0; i < 20; i++) {
            printf("%c", buffer[i]);
        }
        printf("\n");

        if (buffer[0] != 0x47 || buffer[1] != 0x45 || buffer[2] != 0x54) { // !GET
            write(new_socket, HTTP_NOT_FOUND, sizeof(HTTP_NOT_FOUND));
            write(new_socket, CONTENT_TYPE_PLAIN, sizeof(CONTENT_TYPE_PLAIN));
            write(new_socket, "Unsupported", 11);
        } else {
            if (buffer[4] == 0x2F && buffer[5] == 0x20) { // forward slash and space
                printf("Root page, show gameboard\n");
                write(new_socket, HTTP_OK, sizeof(HTTP_OK));
                write(new_socket, CONTENT_TYPE_HTML, sizeof(CONTENT_TYPE_HTML));
                write(new_socket, board, sizeof(board));
            } else if (buffer[4] == 0x2F && buffer[5] == 0x6A && buffer[6] == 0x71 && buffer[7] == 0x75) { // forward slash jqu
                printf("JQuery request\n");
                write(new_socket, HTTP_OK, sizeof(HTTP_OK));
                write(new_socket, CONTENT_TYPE_JAVASCRIPT, sizeof(CONTENT_TYPE_JAVASCRIPT));
                write(new_socket, jquery, sizeof jquery);
            } else if (buffer[4] == 0x2f && buffer[5] == 0x67 && buffer[6] == 0x75 && buffer[7] == 0x65 && buffer[8] == 0x73 && buffer[9] == 0x73) {
                int i, j;
                char word[WORD_SIZE];
                for(i = 0; i < WORD_SIZE; i++) {
                    word[i] = buffer[16+i];
                }
                char response[WORD_SIZE] = {0x63}; // c
                printf("Guess: %s\n", word);
                for(i = 0; i < WORD_SIZE; i++) {
                    if (word[i] == server->secret_word[i]) {
                        response[i] = 0x67; // g
                    } else {
                        for(j = 0; j < WORD_SIZE; j++) {
                            if (word[i] == server->secret_word[j]) {
                                response[i] = 0x79; // y
                            }
                        }
                    }
                }
                write(new_socket, HTTP_OK, sizeof(HTTP_OK));
                write(new_socket, CONTENT_TYPE_PLAIN, sizeof(CONTENT_TYPE_PLAIN));
                write(new_socket, response, WORD_SIZE);
            } else {
                printf("Unknown page\n");
                write(new_socket, HTTP_NOT_FOUND, sizeof(HTTP_NOT_FOUND));
                write(new_socket, CONTENT_TYPE_PLAIN, sizeof(CONTENT_TYPE_PLAIN));
                write(new_socket, "Not found", 9);
            }
        }

#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
        printf("Closing socket.\n");
        close(new_socket);
        new_socket = 0;
#endif
    }
}
