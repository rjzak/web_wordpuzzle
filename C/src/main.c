#include <stdio.h>
#include <stdlib.h>
#include "www.h"

#ifndef __wasi__
#include <signal.h>
void handle_sigint(int sig);
WebServer *server = NULL;
#endif

int main() {
#ifdef __wasi__
    char fdcount[3] = {0}; // FD integer + null terminator
    char *envvar = "FD_COUNT";
    int fd = 3; // Default Fd for the socket
    if (getenv(envvar)) {
        // Check the FD_COUNT environment variable to see the number of FD's
        // Let's assume that we're using the last one
        snprintf(fdcount, 2, "%s", getenv(envvar));
#ifdef DEBUG
        fprintf(stderr, "Got FD_COUNT=%s\n", fdcount);
#endif
        // Minus 1 for zero index
        fd = atoi(fdcount)-1;
#ifdef DEBUG
        fprintf(stderr, "FD=%d\n", fd);
#endif
    }
    WebServer * server = CreateWebServerWithFD(fd);
#else
    signal(SIGINT, handle_sigint);
    server = CreateWebServerWithPort(8080);
#endif
    RunWebServer(server);
    DestroyWebServer(server);
    return EXIT_SUCCESS;
}

#ifndef __wasi__
void handle_sigint(int sig) {
    if (server != NULL)
        DestroyWebServer(server);
#ifdef DEBUG
    printf("Signal caught, exiting gracefully.\n");
#endif
    exit(0);
}
#endif
