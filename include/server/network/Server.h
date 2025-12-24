#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <vector>
#include <netinet/in.h>
#include <sys/poll.h>

#include "network/Client.h"
#include "Parser.h"
#include "MessageType.h"

class Server {
private:
    int socket{};
    sockaddr_in address{};
    std::vector<std::unique_ptr<Client>> clients;
    std::vector<pollfd> pfds;
    Parser *parser;
    void accept_new_client();
    void handle_client(size_t client_index);
    void handle_message(Client* client, Message* message);
    void send_message(const Client* client, Response::Type type, const std::string& payload);
    bool validate_nickname(Client* client, const std::string& nickname);
public:
    Server(int port);
    void run();
    ~Server();
};

#endif