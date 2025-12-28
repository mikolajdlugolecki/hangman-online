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

class Server
{
public:
    explicit Server(int port);
    ~Server();
    void run();
    void sendMessage(const Client* client, Response::Type type, const std::string& payload);
    static void writeDebugLog(const Client *client, const std::string& message);

private:
    int socket{};
    sockaddr_in address{};
    Parser *parser;
    std::vector<std::unique_ptr<Client>> clients;
    std::vector<pollfd> pfds;
    std::vector<std::unique_ptr<Room>> rooms;

    void acceptNewClient();
    void handleClient(size_t client_index);
    void handleMessage(Client* client, const Message* message);
    bool validateNickname(Client* client, const std::string& nickname) const;
    void createNewRoom(Client *client);
    void joinRoom(Client *client, const std::string& id, const std::string& pin);
    void leaveRoom(const Client *client);
    Room* findRoom(const std::string& id) const;
    Room* findRoom(const Client *client) const;
    void startGame(const Client *roomOwner) const;
    void timerThread();
    void checkGuess(Client *client, const std::string &letter);
};

#endif
