#include<stdlib.h>
#include "www.h"

int main() {
#if ((defined(__APPLE__) || defined(__linux__) || defined(__unix__)))
    WebServer * server = CreateWebServerWithPort(8080);
#else
    WebServer * server = CreateWebServerWithFD(3);
#endif
    RunWebServer(server);
    DestroyWebServer(server);
    return EXIT_SUCCESS;
}