#ifndef SERVER_H
#define SERVER_H

#include "Client.h"
#include "Parser.h"
#include "Room.h"

#include <memory>
#include <netinet/in.h>
#include <sys/poll.h>
#include <vector>

class Server
{
public:
    Server(int port, std::atomic<bool> &running);
    ~Server();
    void run();
    void sendBufferData(Client *client) const;

private:
    int socket{};
    sockaddr_in address{};
    Parser *parser;
    std::vector<std::shared_ptr<Client>> clients;
    std::vector<pollfd> pfds;
    std::vector<std::unique_ptr<Room>> rooms;
    std::atomic<bool> &running;

    void secondElapsed() const;

    void acceptNewClient();
    void handleClient(size_t client_index);
    void handleMessage(std::shared_ptr<Client> clientShared, const Message *message);
    bool validateNickname(Client *client, const std::string &nickname) const;
    void createNewRoom(Client *client);
    void joinRoom(Client *client, const std::string &id, const std::string &pin) const;
    void leaveRoom(const std::shared_ptr<Client> client);
    void startGame(const Client *roomOwner) const;
    void timerThread() const;
    void checkGuess(Client *client, const char &letter);
};

#endif
