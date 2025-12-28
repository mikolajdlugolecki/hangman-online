#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <netinet/in.h>

#include "Message.h"
#include "Constants.h"

class Room;

class Server;

class Client {
public:
    int socket;
    sockaddr_in address{};
    Message *message;
    std::vector<char> buffer;

    std::string nickname;
    Room* room = nullptr;

    bool is_connected = true;
    bool received_pong = false;
    int ping_interval_counter_seconds = PING_INTERVAL_SECONDS;
    std::vector<int> pong_timeout_counters_seconds;

    Client(int in_socket, sockaddr_in in_address);
    ~Client();

    std::string address_to_string();
    void tick(Server *server);
};

#endif
