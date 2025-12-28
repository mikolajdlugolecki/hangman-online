#ifndef ROOM_H
#define ROOM_H

#include <random>
#include <vector>

#include "Client.h"
#include "MessageType.h"
#include "Parser.h"

class Game;
class Server;

class Room
{
public:
    std::string id;
    std::string pin;
    Game* game;
    bool isGameStarted = false;

    Room(Server *server, Client *owner);
    ~Room();
    void addClient(Client client);
    void broadcastMessage(Response::Type type, const std::string& payload) const;
    void broadcastPlayersListLobby() const;
    void broadcastPlayersListGame() const;
    void broadcastPlayersGameStats() const;
    void join(Client *client);
    Client* leave(const Client *client);
    bool isClientInRoom(const Client *client) const;
    void startGame();

private:
    Client *owner;
    Server *server;
    std::vector<Client*> clients;
    static inline int roomCounter = 1;

    static std::string generatePin();
};  

#endif
