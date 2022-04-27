#include <stdio.h>
#include <stdlib.h>
#include "www.h"

int main() {
#ifdef __wasi__
    char fdcount[2]; // FD integer + null terminator
    char *envvar = "FD_COUNT";
    int fd = 3; // Default Fd for the socket
    if (getenv(envvar)) {
        // Check the FD_COUNT environment variable to see the number of FD's
        // Let's assume that we're using the last one
        snprintf(fdcount, 2, "%s", getenv(envvar));
#ifdef DEBUG
        printf("Got FD_COUNT=%s\n", fdcount);
#endif
        // Minus 1 for zero index
        fd = atoi(fdcount)-1;
#ifdef DEBUG
        printf("FD=%d\n", fd);
#endif
    }
    WebServer * server = CreateWebServerWithFD(fd);
#else
    WebServer * server = CreateWebServerWithPort(8080);

#endif
    RunWebServer(server);
    DestroyWebServer(server);
    return EXIT_SUCCESS;
}