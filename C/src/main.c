#include<stdio.h>
#include<stdlib.h>
#include "www.h"

int main() {
    WebServer * server = CreateWebServer(8080);
    RunWebServer(server);
    DestroyWebServer(server);
    return EXIT_SUCCESS;
}