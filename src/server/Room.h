#ifndef ROOM_H
#define ROOM_H

#include "Client.h"
#include "GameStats.h"
#include "MessageType.h"

#include <memory>
#include <unordered_map>
#include <vector>

class Game;
class Server;

class Room
{
public:
    std::string id;
    std::string pin;
    std::unique_ptr<Game> game;
    std::unordered_map<Client *, std::shared_ptr<GameStats>> gameStats;
    std::chrono::steady_clock::time_point lastUpdate;

    Room(Server *server, Client *owner);
    ~Room();
    void broadcastMessage(ServerMessageTypes::Type type, const std::string &payload) const;
    void broadcastPlayersListLobby() const;
    void broadcastPlayersListGame() const;
    void broadcastPlayersGameStats() const;
    void join(Client *client);
    Client *leave(const Client *client);
    bool isClientInRoom(const Client *client) const;
    void startGame();
    void updateGame();
    bool allClientsFinished() const;
    std::string getStats(Client *client);

private:
    Client *owner;
    Server *server;
    std::vector<Client *> clients;
    static inline int roomCounter = 1;

    static std::string generatePin();
    std::string getGameResult();
};

#endif
