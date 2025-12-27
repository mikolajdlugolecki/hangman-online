#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <vector>
#include <netinet/in.h>
#include <sys/poll.h>

#include "Client.h"
#include "MessageType.h"
#include "Parser.h"
#include "Room.h"

class Server {
private:
    int socket{};
    sockaddr_in address{};
    Parser *parser;

    std::vector<std::unique_ptr<Client>> clients;
    std::vector<pollfd> pfds;
    std::vector<std::unique_ptr<Room>> rooms;

    void accept_new_client();
    void handle_client(size_t client_index);
    void handle_message(Client* client, Message* message);
    bool validate_nickname(Client* client, const std::string& nickname);
    void create_new_room(Client *client);
    void join_room(Client *client, std::string id, std::string pin);
    void leave_room(Client *client);
    Room* find_room(std::string id);
    Room* find_room(Client *client);
    void start_game(Client *client);

public:
    Server(int port);
    void run();
    ~Server();

    void send_message(const Client* client, Response::Type type, const std::string& payload);
    void write_debug_log(Client *client, std::string message);
};

#endif
