#include<stdlib.h>
#include "www.h"

int main() {
#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
    WebServer * server = CreateWebServerWithPort(8080);
#else
    char fdcount[2]; // FD integer + null terminator
    char *envvar = "FD_COUNT";
    int fd = 3; // Default Fd for the socket
    if (getenv(envvar)) {
        // Check the FD_COUNT environment variable to see the number of FD's
        // Let's assume that we're using the last one
        snprintf(fdcount, 2, "%s", getenv(envvar));
        //printf("Got FD_COUNT=%s\n", fdcount);
        // Minus 1 for zero index
        fd = atoi(fdcount)-1;
        //printf("FD=%d\n", fd);
    }
    WebServer * server = CreateWebServerWithFD(fd);
#endif
    RunWebServer(server);
    DestroyWebServer(server);
    return EXIT_SUCCESS;
}