#include "www.h"
#include "assets.h"
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const unsigned char HTTP_OK[13] = "HTTP/1.0 200\n";
const unsigned char HTTP_NOT_FOUND[13] = "HTTP/1.0 403\n";
const unsigned char CONTENT_TYPE_HTML[25] = "Content-Type: text/html\n\n";
const unsigned char CONTENT_TYPE_PLAIN[26] = "Content-Type: text/plain\n\n";
const unsigned char CONTENT_TYPE_JAVASCRIPT[31] = "Content-Type: text/javascript\n\n";

#define BUFF_LEN 5000

#ifdef __wasi__
// WebAssembly requires use of an existing socket descriptor
WebServer* CreateWebServerWithFD(int sd) {
#else
WebServer* CreateWebServerWithPort(uint16_t port) {
#endif

    // Better random int with stdlib
    // https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c
    struct timespec nanos;
    clock_gettime(CLOCK_MONOTONIC, &nanos);
    srand(nanos.tv_nsec);

    WebServer *server = (WebServer*) malloc(sizeof(WebServer));
    if (server == NULL) {
        perror("In malloc");
        exit(EXIT_FAILURE);
    }

    unsigned int randIndex = rand() % (sizeof(wordlist)/6u);
#ifdef DEBUG
    printf("randIndex: %d\nsizeof(wordlist): %ld\nNum words: %ld\n", randIndex, sizeof(wordlist), sizeof(wordlist)/6u);
#endif
    while(wordlist[randIndex] != 0x0A) {
        randIndex++;
    }
    randIndex++;
    unsigned int i;
    for(i = 0; i < WORD_SIZE; i++) {
        server->secret_word[i] = tolower(wordlist[randIndex+i]);
    }
    printf("The word: %s\n", server->secret_word);

#ifdef __wasi__
    server->server_fd = sd;
    server->address.sin_port = htons(8080);
#else
    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }
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
#endif

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;

    return server;
}

void DestroyWebServer(WebServer* server) {
#ifndef __wasi__
    close(server->server_fd);
    server->server_fd = 0;
#endif
    free(server);
    server = NULL;
}

void RunWebServer(WebServer *server) {
#ifndef __wasi__
    int addrlen = sizeof(server->address);
#endif
    char buffer[BUFF_LEN] = {0};
    printf("Listening for connections\n");
    while(1) {
        int new_socket;

#ifdef __wasi__
        if ((new_socket = __wasi_sock_accept(server->server_fd, SOCK_NONBLOCK, 0))<0) {
#else
        if ((new_socket = accept(server->server_fd, (struct sockaddr *)&server->address, (socklen_t*)&addrlen))<0) {
#endif
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        memset(buffer,0,BUFF_LEN);
        errno = 0;
        ssize_t bytes_read = read(new_socket ,buffer,BUFF_LEN);
        if (bytes_read < 0) {
            fprintf(stderr, "Error code %ld from read(%d): %s\n", bytes_read, new_socket, strerror(errno));
            return;
        }
        int i;
#ifdef DEBUG
        printf("First 20 bytes: ");
        for(i = 0; i < 20; i++) {
            printf("%c", buffer[i]);
        }
        printf("\n");
#endif

        if (buffer[0] != 0x47 || buffer[1] != 0x45 || buffer[2] != 0x54) { // !GET
            write(new_socket, HTTP_NOT_FOUND, sizeof(HTTP_NOT_FOUND));
            write(new_socket, CONTENT_TYPE_PLAIN, sizeof(CONTENT_TYPE_PLAIN));
            write(new_socket, "Unsupported", 11);
        } else {
            if (buffer[4] == 0x2F && buffer[5] == 0x20) { // forward slash and space
#ifdef DEBUG
                printf("Root page, show gameboard\n");
#endif
                write(new_socket, HTTP_OK, sizeof(HTTP_OK));
                write(new_socket, CONTENT_TYPE_HTML, sizeof(CONTENT_TYPE_HTML));
                write(new_socket, board, sizeof(board));
            } else if (buffer[4] == 0x2F && buffer[5] == 0x6A && buffer[6] == 0x71 && buffer[7] == 0x75) { // forward slash jqu
#ifdef DEBUG
                printf("JQuery request\n");
#endif
                write(new_socket, HTTP_OK, sizeof(HTTP_OK));
                write(new_socket, CONTENT_TYPE_JAVASCRIPT, sizeof(CONTENT_TYPE_JAVASCRIPT));
                write(new_socket, jquery, sizeof jquery);
            } else if (buffer[4] == 0x2f && buffer[5] == 0x67 && buffer[6] == 0x75 && buffer[7] == 0x65 && buffer[8] == 0x73 && buffer[9] == 0x73) {
                int j;
                char word[WORD_SIZE+1] = {0x0};
                for(i = 0; i < WORD_SIZE; i++) {
                    word[i] = buffer[16+i];
                }
                char response[WORD_SIZE] = {0x63}; // c
#ifdef DEBUG
                printf("Guess: %s\n", word);
#endif
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

#ifndef __wasi__
#ifdef DEBUG
        printf("Closing socket.\n");
#endif
        close(new_socket);
        new_socket = 0;
#endif
    }
}
