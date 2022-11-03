#ifndef _SERVER_H__
#define _SERVER_H__

#include "WordGame.h"
#include <string>
using namespace std;

const unsigned char HTTP_OK[14] = "HTTP/1.0 200\n";
const unsigned char HTTP_NOT_FOUND[14] = "HTTP/1.0 404\n";
const unsigned char CONTENT_TYPE_HTML[26] = "Content-Type: text/html\n\n";
const unsigned char CONTENT_TYPE_PLAIN[27] = "Content-Type: text/plain\n\n";
const unsigned char CONTENT_TYPE_JAVASCRIPT[32] = "Content-Type: text/javascript\n\n";


class Server {
public:
    Server(int fd);
    Server(string host, int port);

    void listen();

private:
    WordGame wg;
};

#endif