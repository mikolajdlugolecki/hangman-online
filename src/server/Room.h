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
    std::unordered_map<std::shared_ptr<Client>, std::shared_ptr<GameStats>> gameStats;
    std::chrono::steady_clock::time_point lastUpdate;

    Room(Server *server, const std::shared_ptr<Client>& owner);
    ~Room();
    void broadcastMessage(ServerMessageTypes::Type type, const std::string &payload) const;
    void broadcastPlayersListLobby() const;
    void broadcastPlayersListGame() const;
    void broadcastPlayersGameStats() const;
    void join(const std::shared_ptr<Client>& client);
    std::shared_ptr<Client> leave(const std::shared_ptr<Client> &client);
    bool isClientInRoom(const std::shared_ptr<Client>& client) const;
    void startGame();
    void updateGame();
    bool allClientsFinished() const;
    std::string getStats(const std::shared_ptr<Client>& client);

private:
    std::shared_ptr<Client> owner;
    Server *server;
    std::vector<std::shared_ptr<Client>> clients;
    static inline int roomCounter = 1;
    std::vector<std::shared_ptr<Client>> disconnectedClients;

    static std::string generatePin();
    std::string getGameResult();
};

#endif
