#ifndef www_h__
#define www_h__

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>

// Modelled after
// https://github.com/Dungyichao/http_server/blob/master/src/helloworld.cpp

// typedef unsigned char byte;
// typedef void (*web_handler)(*byte, *uint32_t, **byte, *uint32_t, byte[10]); // request, request size, response, response size, type

#define WORD_SIZE 5

typedef struct {
    char secret_word[WORD_SIZE];
    int server_fd;
    struct sockaddr_in address;
} WebServer;

WebServer* CreateWebServer(uint16_t port);
void DestroyWebServer(WebServer* server);
void RunWebServer(WebServer *server);

#endif