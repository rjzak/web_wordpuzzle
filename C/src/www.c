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
const unsigned char HTTP_NOT_FOUND[13] = "HTTP/1.0 404\n";
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
    socklen_t addrlen = 0;
    char buffer[BUFF_LEN] = {0};
    int new_socket;
    printf("Listening for connections\n");
    fflush(stdout);
    while(1) {
#ifdef DEBUG
        printf("Calling accept()\n");
        fflush(stdout);
#endif
        if ((new_socket = accept(server->server_fd, (struct sockaddr *)&server->address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            return;
        }

        memset(buffer,0,BUFF_LEN);
        errno = 0;
#ifdef DEBUG
        printf("About to read from socket.\n");
        fflush(stdout);
#endif
        ssize_t bytes_read = read(new_socket ,buffer,BUFF_LEN);
        if (bytes_read < 0) {
            fprintf(stderr, "Error code %ld from read(%d): %s\n", bytes_read, new_socket, strerror(errno));
            close(new_socket);
            return;
        }
        int i;
#ifdef DEBUG
        printf("Read from socket, first 20 bytes: ");
        fflush(stdout);
        for(i = 0; i < 20; i++) {
            printf("%c", buffer[i]);
        }
        printf("\n");
        fflush(stdout);
#endif

        if (buffer[0] != 0x47 || buffer[1] != 0x45 || buffer[2] != 0x54) { // !GET
            write(new_socket, HTTP_NOT_FOUND, sizeof(HTTP_NOT_FOUND));
            write(new_socket, CONTENT_TYPE_PLAIN, sizeof(CONTENT_TYPE_PLAIN));
            write(new_socket, "Unsupported", 11);
#ifdef DEBUG
            printf("Closing socket.\n");
            fflush(stdout);
#endif
            close(new_socket);
            new_socket = 0;
            continue;
        }

        if (buffer[4] == 0x2F && buffer[5] == 0x20) { // forward slash and space
#ifdef DEBUG
            printf("Root page, show gameboard\n");
            fflush(stdout);
#endif
            write(new_socket, HTTP_OK, sizeof(HTTP_OK));
            write(new_socket, CONTENT_TYPE_HTML, sizeof(CONTENT_TYPE_HTML));
            write_all(new_socket, board, sizeof(board));
#ifdef DEBUG
            printf("Closing socket.\n");
            fflush(stdout);
#endif
            close(new_socket);
            new_socket = 0;
            continue;
        }

        if (buffer[4] == 0x2F && buffer[5] == 0x6A && buffer[6] == 0x71 && buffer[7] == 0x75) { // forward slash jqu
#ifdef DEBUG
            printf("JQuery requested\n");
            fflush(stdout);
#endif
            write(new_socket, HTTP_OK, sizeof(HTTP_OK));
            write(new_socket, CONTENT_TYPE_JAVASCRIPT, sizeof(CONTENT_TYPE_JAVASCRIPT));
            write_all(new_socket, jquery, sizeof(jquery));
#ifdef DEBUG
            printf("Closing socket.\n");
            fflush(stdout);
#endif
            close(new_socket);
            new_socket = 0;
            continue;
        }

        if (buffer[4] == 0x2f && buffer[5] == 0x67 && buffer[6] == 0x75 && buffer[7] == 0x65 && buffer[8] == 0x73 && buffer[9] == 0x73) {
            int j;
            char *word = buffer + 16;
            char response[WORD_SIZE];
            for(i = 0; i < WORD_SIZE; i++) {
                response[i] = 'c';
            }
#ifdef DEBUG
            printf("Guess: ");
            for(i = 0; i < WORD_SIZE; i++) {
                printf("%c", word[i]);
            }
            printf("\n");
            fflush(stdout);
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
#ifdef DEBUG
            printf("Closing socket.\n");
            fflush(stdout);
#endif
            close(new_socket);
            new_socket = 0;
            continue;
        }

        // If we've gotten this far, we don't know what to do with the request, so return 404
        printf("Unknown page %c%c%c%c\n", buffer[5], buffer[6], buffer[7], buffer[8]);
        write(new_socket, HTTP_NOT_FOUND, sizeof(HTTP_NOT_FOUND));
        write(new_socket, CONTENT_TYPE_PLAIN, sizeof(CONTENT_TYPE_PLAIN));
        write(new_socket, "Not found", 9);
        fflush(stdout);
#ifdef DEBUG
        printf("Closing socket.\n");
        fflush(stdout);
#endif
        close(new_socket);
        new_socket = 0;
    } // End while-loop
}

ssize_t write_all(const int fd, const void *buf, const size_t n) {
    size_t total_written = 0;
    while(total_written < n) {
        size_t written = write(fd, buf+total_written, n-total_written);
        if (written < 0)
            return written;
        total_written += written;
    }
    return total_written;
}
