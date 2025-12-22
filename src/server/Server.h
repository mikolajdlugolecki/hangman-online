#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <vector>
#include <netinet/in.h>
#include <sys/poll.h>

#include "Client.h"
#include "../common/Parser.h"

class Server {
private:
    int socket{};
    sockaddr_in address{};
    std::vector<std::unique_ptr<Client>> clients;
    std::vector<pollfd> pfds;
    Parser *parser;
    void setup(int port);
public:
    Server(int port);
    void run();
    ~Server();
};

#endif